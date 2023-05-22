// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/GAICharacter.h"

#include "AIController.h"
#include "BrainComponent.h"
#include "DrawDebugHelpers.h"
#include "AI/BTTask_RecoverHealth.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/GActionComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/GameplayStatics.h"
#include "UI/GUserWidget_World.h"

// Sets default values
AGAICharacter::AGAICharacter()
{
	AttributeComp = CreateDefaultSubobject<UGAttributeComponent>("AttributeComp");
	PawnSensingComp = CreateDefaultSubobject<UPawnSensingComponent>("PawnSensingComp");
	ActionComp = CreateDefaultSubobject<UGActionComponent>("ActionComp");
	AutoPossessAI = EAutoPossessAI::PlacedInWorldOrSpawned;
	// Temporary
	GetCapsuleComponent()->SetCollisionResponseToChannel(ECC_WorldDynamic,ECR_Ignore);
	GetMesh()->SetGenerateOverlapEvents(true);
	

	RandomAttackOffset = 2.0f;
	TimeToHit = "TimeToHit";
	TargetActorKey = "TargetActor";
}


void AGAICharacter::SpawnProjectile(TSubclassOf<AActor> Projectile,FVector Location,FRotator Rotation)
{
	if(ensureAlways(Projectile))
	{
		FActorSpawnParameters SpawnParams;
		SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
		SpawnParams.Instigator = this;

		FTransform SpawnTrans = FTransform(Rotation,Location);

		GetWorld()->SpawnActor<AActor>(Projectile,SpawnTrans,SpawnParams);
	}
}


void AGAICharacter::Die()
{
	// stop BT
	AAIController* AIC = Cast<AAIController>(GetController());
	if(AIC)
	{
		AIC->GetBrainComponent()->StopLogic("Killed");
	}
	// Ragdoll
	GetMesh()->SetAllBodiesSimulatePhysics(true);
	GetMesh()->SetCollisionProfileName("Ragdoll"); 
	// Set Capsule Collision correctly
	GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	GetCharacterMovement()->DisableMovement();
	// SetLifeSpan
	SetLifeSpan(10.0f);
}

void AGAICharacter::SetTargetActor(AActor* Target)
{
	AAIController* AIC = Cast<AAIController>(GetController());
	if(AIC)
	{
		auto MyBB = AIC->GetBlackboardComponent();
		MyBB->SetValueAsObject(TargetActorKey,Target);
		DisplaySpottedPlayerWidget();
		//DrawDebugString(GetWorld(),GetActorLocation(),"Change TargetActor!!!",nullptr,FColor::White,4.0f,true);
	}
}

void AGAICharacter::DisplaySpottedPlayerWidget()
{
	if(!PlayerSpottedWidgetInstance && ensure(PlayerSpottedWidgetClass))
	{
		PlayerSpottedWidgetInstance = CreateWidget<UGUserWidget_World>(GetWorld(),PlayerSpottedWidgetClass);
	}
	if(PlayerSpottedWidgetInstance)
	{
		PlayerSpottedWidgetInstance->AttachedActor = this;
		if(!PlayerSpottedWidgetInstance->IsInViewport())
		{
			PlayerSpottedWidgetInstance->AddToViewport(10);
		}
	}
}


void AGAICharacter::PostInitializeComponents()
{
	Super::PostInitializeComponents();

	PawnSensingComp->OnSeePawn.AddDynamic(this,&AGAICharacter::OnPawnSeen);
	AttributeComp->OnHealthChanged.AddDynamic(this,&AGAICharacter::OnChangedHealth);
}

void AGAICharacter::OnPawnSeen(APawn* SeenPawn)
{
	AActor* PrevTarget = GetTargetActor();
	if(PrevTarget!=SeenPawn)
	{
		SetTargetActor(SeenPawn);
	}
	//DrawDebugString(GetWorld(),GetActorLocation(),"See Player!!!",nullptr,FColor::White,4.0f,true);
}

void AGAICharacter::OnChangedHealth(AActor* InstigatorActor, UGAttributeComponent* OwningComp, float NewHealth,
	float Delta)
{
	if(Delta<0.0f)
	{
		DisplayHealthBar();
		DamagedFlash();
		// can be improve by filter out ally
		if(InstigatorActor!=this)
		{
			SetTargetActor(InstigatorActor);
		}
		if(NewHealth<=0)
		{
			Die();
		}
	}
}

float AGAICharacter::GetAttackRange_Implementation()
{
	return AttackRange;
}

void AGAICharacter::Attack_Implementation(AActor* TargetActor)
{
	IGAICharacterInterface::Attack_Implementation(TargetActor);

	FVector SpawnLocation = GetMesh()->GetSocketLocation("Muzzle_Front_XForward");
	FVector TargetLocation = TargetActor->GetActorLocation();
	FRotator SpawnRotation = FRotationMatrix::MakeFromX(TargetLocation-SpawnLocation).Rotator();
	// Give random offset to projectile to make game more fun
	SpawnRotation.Pitch += FMath::RandRange(0.0f,RandomAttackOffset);
	SpawnRotation.Yaw += FMath::RandRange(-RandomAttackOffset,RandomAttackOffset);
	
	auto RangeAttack_TimeElapsed = [=]()
	{
		SpawnProjectile(RangeAttackProjectile,SpawnLocation,SpawnRotation);
	};
	PlayAnimMontage(RangeAttackAnim);
	GetWorldTimerManager().SetTimer(TimerHandle_RangeAttack,RangeAttack_TimeElapsed,1,false,RangeAttackProjectileDelay);
}

void AGAICharacter::Recover_Implementation(UBTTaskNode* TaskNode)
{
	IGAICharacterInterface::Recover_Implementation(TaskNode);

	float AnimLength = PlayAnimMontage(RecoverAnim,0.5) * 2;
	/*if(GEngine)
	{
		GEngine->AddOnScreenDebugMessage(0,10,FColor::Green,FString::SanitizeFloat(AnimLength));
	}*/

	UBTTask_RecoverHealth* RecoverHealthTask = Cast<UBTTask_RecoverHealth>(TaskNode);
	auto FinishRecover = [=]()		
	{
		if(IsValid(AttributeComp))
		{
			AttributeComp->ApplyHealthChange(this,AttributeComp->GetMaxHealth());
		}
		if(RecoverHealthTask)
		{
			RecoverHealthTask->OnRecoverFinished.Broadcast();
		}
	};
	GetWorldTimerManager().SetTimer(TimerHandle_Recover,FinishRecover,AnimLength,false);
}

void AGAICharacter::DamagedFlash()
{
	GetMesh()->SetScalarParameterValueOnMaterials(TimeToHit,GetWorld()->TimeSeconds);
}

void AGAICharacter::DisplayHealthBar()
{
	if(!HealthBarInstance)
	{
		HealthBarInstance = CreateWidget<UGUserWidget_World>(GetWorld(),HealthBarClass);
		HealthBarInstance->AttachedActor = this;
		HealthBarInstance->AddToViewport();
	}
}

AActor* AGAICharacter::GetTargetActor() const
{
	AAIController* AIC = Cast<AAIController>(GetController());
	{
		auto MyBB = AIC->GetBlackboardComponent();
		return Cast<AActor>(MyBB->GetValueAsObject(TargetActorKey));
		//DrawDebugString(GetWorld(),GetActorLocation(),"Change TargetActor!!!",nullptr,FColor::White,4.0f,true);
	}
	return nullptr;
}
