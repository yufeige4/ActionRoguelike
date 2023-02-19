// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GProjectileBase.h"
#include "Sound/SoundCue.h"
#include "GMagicProjectile.generated.h"

UCLASS()
class ACTIONROGUELIKE_API AGMagicProjectile : public AGProjectileBase
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AGMagicProjectile();
	
	virtual void BeginPlay() override;


protected:
	
	UPROPERTY(EditDefaultsOnly, Category = "Damage")
	float DamageAmount;

	UPROPERTY(EditDefaultsOnly, Category = "Basic")
	float TimeToSelfDestroy;

	UPROPERTY(EditDefaultsOnly, Category = "Audio")
	USoundCue* ImpactSound;

	UPROPERTY(EditDefaultsOnly, Category = "Effects")
	TSubclassOf<UCameraShakeBase> ImpactShake;
	

	FTimerHandle TimerHandle_selfDestroy;

	void SelfDestroy();

	virtual void Explode_Implementation() override;
	
	UFUNCTION()
	void OnActorOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
};

