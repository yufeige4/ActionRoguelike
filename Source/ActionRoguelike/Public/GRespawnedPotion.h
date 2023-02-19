// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "RespawnedInteractableBase.h"
#include "Sound/SoundCue.h"
#include "GRespawnedPotion.generated.h"

/**
 * 
 */
UCLASS()
class ACTIONROGUELIKE_API AGRespawnedPotion : public ARespawnedInteractableBase
{
	GENERATED_BODY()

public:
	AGRespawnedPotion();

protected:

	UPROPERTY(EditAnywhere, Category = "Design")
	float HealingAmount;

	UPROPERTY(EditDefaultsOnly, Category = "Design")
	UParticleSystem* HealingEffect;

	UPROPERTY(EditDefaultsOnly, Category = "Audio")
	USoundCue* HealingSound;
	

	virtual void Interact_Implementation(APawn* InstigatorPawn) override;
	
	
	
};
