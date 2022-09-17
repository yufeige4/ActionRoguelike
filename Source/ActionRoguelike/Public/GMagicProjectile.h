// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GProjectileBase.h"
#include "GMagicProjectile.generated.h"

UCLASS()
class ACTIONROGUELIKE_API AGMagicProjectile : public AGProjectileBase
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AGMagicProjectile();

protected:
	
	UPROPERTY(EditDefaultsOnly, Category = "Damage")
	float DamageAmount;

	UFUNCTION()
	void OnActorOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
};
