// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/GAICharacter.h"

#include "AIController.h"
#include "DrawDebugHelpers.h"
#include "BehaviorTree/BlackboardComponent.h"

// Sets default values
AGAICharacter::AGAICharacter()
{

	PawnSensingComp = CreateDefaultSubobject<UPawnSensingComponent>("PawnSensingComp");
	
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


void AGAICharacter::PostInitializeComponents()
{
	Super::PostInitializeComponents();

	PawnSensingComp->OnSeePawn.AddDynamic(this,&AGAICharacter::OnPawnSeen);
}

void AGAICharacter::OnPawnSeen(APawn* SeenPawn)
{
	AAIController* AIC = Cast<AAIController>(GetController());
	if(AIC)
	{
		auto MyBB = AIC->GetBlackboardComponent();
		MyBB->SetValueAsObject("TargetActor",SeenPawn);

		DrawDebugString(GetWorld(),GetActorLocation(),"See Player!!!",nullptr,FColor::White,4.0f,true);
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

	auto RangeAttack_TimeElapsed = [=]()
	{
		SpawnProjectile(RangeAttackProjectile,SpawnLocation,SpawnRotation);
	};
	PlayAnimMontage(RangeAttackAnim);
	GetWorldTimerManager().SetTimer(TimerHandle_RangeAttack,RangeAttack_TimeElapsed,1,false,RangeAttackProjectileDelay);
}


