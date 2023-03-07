// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GAICharacterInterface.h"
#include "Blueprint/UserWidget.h"
#include "Components/GAttributeComponent.h"
#include "GameFramework/Character.h"
#include "Perception/PawnSensingComponent.h"
#include "GAICharacter.generated.h"

UCLASS()
class ACTIONROGUELIKE_API AGAICharacter : public ACharacter, public IGAICharacterInterface
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	AGAICharacter();

protected:

	UPROPERTY(VisibleAnywhere, Category = "Components")
	UGAttributeComponent* AttributeComp;
	UPROPERTY(VisibleAnywhere, Category = "Components")
	UPawnSensingComponent* PawnSensingComp;

	UPROPERTY(EditAnywhere, Category = "AI|Attack")
	float AttackRange = 500;

	UPROPERTY(EditAnywhere, Category = "AI|Attack")
	TSubclassOf<AActor> RangeAttackProjectile;

	// Random Rotation Offset in Degree
	UPROPERTY(EditAnywhere, Category = "AI|Attack")
	float RandomAttackOffset;
	
	UPROPERTY(EditDefaultsOnly, Category = "AI|Animation")
	UAnimMontage* RangeAttackAnim;
	
	UPROPERTY(EditDefaultsOnly, Category = "AI|Animation")
	float RangeAttackProjectileDelay;

	UPROPERTY(VisibleDefaultsOnly,Category = "Effects")
	FName TimeToHit;
	
	FTimerHandle TimerHandle_RangeAttack;

public:	 
	
	virtual float GetAttackRange_Implementation() override;
	
	virtual void Attack_Implementation(AActor* TargetActor) override;


protected:
	
	virtual void PostInitializeComponents() override;

	UFUNCTION()
	void OnPawnSeen(APawn* SeenPawn);

	UFUNCTION()
	void OnChangedHealth(AActor* InstigatorActor, UGAttributeComponent* OwningComp, float NewHealth, float Delta);

	void SpawnProjectile(TSubclassOf<AActor> Projectile,FVector Position,FRotator Rotation);

	void DamagedFlash();

	void Die();

	void SetTargetActor(AActor* Target);
};

