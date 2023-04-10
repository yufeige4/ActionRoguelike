// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Abilities/GAction.h"
#include "Particles/ParticleSystemComponent.h"
#include "GAction_ProjectileAttack.generated.h"

class AGCharacter;
/**
 * 
 */
UCLASS()
class ACTIONROGUELIKE_API UGAction_ProjectileAttack : public UGAction
{
	GENERATED_BODY()

protected:

	UPROPERTY(EditAnywhere,Category = "Attack")
	TSubclassOf<AActor> ProjectileClass;
	
	UPROPERTY(EditDefaultsOnly, Category = "Attack")
	float AttackAnimDelay;
	
	UPROPERTY(EditAnywhere,Category = "Attack")
	UAnimMontage* AttackAnim;

	UPROPERTY(VisibleAnywhere, Category = "Effects")
	FName HandSocketName;

	UPROPERTY(EditAnywhere, Category = "Effects")
	UParticleSystem* CastingEffect;
	
public:

	UGAction_ProjectileAttack();
	
	virtual void StartAction_Implementation(AActor* Instigator) override;
	
protected:

	UFUNCTION()
	void AttackDelay_Elapsed(AGCharacter* InstigatorCharacter,UParticleSystemComponent* CastingVFX);

};
