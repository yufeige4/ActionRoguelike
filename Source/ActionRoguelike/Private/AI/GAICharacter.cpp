// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/GAICharacter.h"

#include "AIController.h"
#include "BrainComponent.h"
#include "DrawDebugHelpers.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Components/CapsuleComponent.h"

// Sets default values
AGAICharacter::AGAICharacter()
{
	AttributeComp = CreateDefaultSubobject<UGAttributeComponent>("AttributeComp");
	PawnSensingComp = CreateDefaultSubobject<UPawnSensingComponent>("PawnSensingComp");
	AutoPossessAI = EAutoPossessAI::PlacedInWorldOrSpawned;
	RandomAttackOffset = 2.0f;

	TimeToHit = "TimeToHit";
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

void AGAICharacter::DamagedFlash()
{
	GetMesh()->SetScalarParameterValueOnMaterials(TimeToHit,GetWorld()->TimeSeconds);
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
	Cast<UCapsuleComponent>(RootComponent)->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		
	// SetLifeSpan
	SetLifeSpan(10.0f);
}

void AGAICharacter::SetTargetActor(AActor* Target)
{
	AAIController* AIC = Cast<AAIController>(GetController());
	if(AIC)
	{
		auto MyBB = AIC->GetBlackboardComponent();
		MyBB->SetValueAsObject("TargetActor",Target);
		//DrawDebugString(GetWorld(),GetActorLocation(),"Change TargetActor!!!",nullptr,FColor::White,4.0f,true);
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
	SetTargetActor(SeenPawn);
	//DrawDebugString(GetWorld(),GetActorLocation(),"See Player!!!",nullptr,FColor::White,4.0f,true);
}

void AGAICharacter::OnChangedHealth(AActor* InstigatorActor, UGAttributeComponent* OwningComp, float NewHealth,
	float Delta)
{
	if(Delta<0.0f)
	{
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

void AGAICharacter::Recover_Implementation()
{
	IGAICharacterInterface::Recover_Implementation();

	float AnimLength = PlayAnimMontage(RecoverAnim,0.5);
	
	GetWorldTimerManager().SetTimer(TimerHandle_Recover,this,&AGAICharacter::RecoverElapsed,AnimLength);
}

void AGAICharacter::RecoverElapsed()
{
	AttributeComp->ApplyHealthChange(this,AttributeComp->GetMaxHealth());
	UE_LOG(LogTemp,Log,TEXT("Finish Recover!!!"));
}



