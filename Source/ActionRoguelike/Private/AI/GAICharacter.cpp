// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/GAICharacter.h"

// Sets default values
AGAICharacter::AGAICharacter()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void AGAICharacter::BeginPlay()
{
	Super::BeginPlay();
	
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


// Called every frame
void AGAICharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

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


