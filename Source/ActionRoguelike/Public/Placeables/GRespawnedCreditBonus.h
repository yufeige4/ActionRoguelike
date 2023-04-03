// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Placeables/RespawnedInteractableBase.h"
#include "GRespawnedCreditBonus.generated.h"

class USoundCue;
/**
 * 
 */
UCLASS()
class ACTIONROGUELIKE_API AGRespawnedCreditBonus : public ARespawnedInteractableBase
{
	GENERATED_BODY()

protected:
	
	UPROPERTY(EditDefaultsOnly, Category = "Design")
	UParticleSystem* PickUpEffect;

	UPROPERTY(EditDefaultsOnly, Category = "Audio")
	USoundCue* PickUpSound;
	
	virtual void Interact_Implementation(APawn* InstigatorPawn) override;
};
