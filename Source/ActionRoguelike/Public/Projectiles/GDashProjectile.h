// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GProjectileBase.h"
#include "GDashProjectile.generated.h"

/**
 * 
 */
UCLASS()
class ACTIONROGUELIKE_API AGDashProjectile : public AGProjectileBase
{
	GENERATED_BODY()

protected:

	UPROPERTY(EditDefaultsOnly, Category = "Teleport")
	float DetonateDelay;

	UPROPERTY(EditDefaultsOnly, Category = "Teleport")
	float TeleportDelay;

	FTimerHandle TimerHandle_DetonateDelay;

	// overwirte the implement of Explode function
	virtual void Explode_Implementation() override;

	void Teleport();

	virtual void BeginPlay() override;

	
public:
	AGDashProjectile();
};
