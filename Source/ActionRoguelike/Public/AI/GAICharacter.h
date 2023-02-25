// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GAICharacterInterface.h"
#include "GameFramework/Character.h"
#include "GAICharacter.generated.h"

UCLASS()
class ACTIONROGUELIKE_API AGAICharacter : public ACharacter, public IGAICharacterInterface
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	AGAICharacter();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UPROPERTY(EditAnywhere, Category = "AI|Attack")
	float AttackRange = 500;

	UPROPERTY(EditAnywhere, Category = "AI|Attack")
	TSubclassOf<AActor> RangeAttackProjectile;

	UPROPERTY(EditDefaultsOnly, Category = "AI|Animation")
	UAnimMontage* RangeAttackAnim;
	UPROPERTY(EditDefaultsOnly, Category = "AI|Animation")
	float RangeAttackProjectileDelay;

	void SpawnProjectile(TSubclassOf<AActor> Projectile,FVector Position,FRotator Rotation);

	FTimerHandle TimerHandle_RangeAttack;
	void RangeAttack_TimeElapsed();
	
	
public:	 
	// Called every frame
	virtual void Tick(float DeltaTime) override;
	
	virtual float GetAttackRange_Implementation() override;
	
	virtual void Attack_Implementation(AActor* TargetActor) override;
};
