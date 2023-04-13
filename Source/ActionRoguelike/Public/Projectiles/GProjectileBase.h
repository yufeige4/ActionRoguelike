// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "GProjectileBase.generated.h"

class UParticleSystem;
class UProjectileMovementComponent;
class UParticleSystemComponent;
class UAudioComponent;
class USphereComponent;

UCLASS(Abstract)
class ACTIONROGUELIKE_API AGProjectileBase : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AGProjectileBase();

protected:

	UPROPERTY(EditDefaultsOnly, Category = "Basics")
	float ProjectileSpeed = 2000;
	// 爆炸特效
	UPROPERTY(EditDefaultsOnly, Category = "Effects")
	UParticleSystem* ImpactVFX;
	
	UPROPERTY(VisibleAnywhere,BlueprintReadOnly, Category = "Components")
	USphereComponent* SphereComp;

	UPROPERTY(VisibleAnywhere,BlueprintReadOnly, Category = "Components")
	UProjectileMovementComponent* MoveComp;

	UPROPERTY(VisibleAnywhere,BlueprintReadOnly, Category = "Components")
	UParticleSystemComponent* EffectComp;

	// 飞行音效
	UPROPERTY(VisibleAnywhere, Category = "Components")
	UAudioComponent* AudioComp;

	// implement on-hit event
	UFUNCTION()
	virtual void OnActorHit(UPrimitiveComponent* HitComp,AActor* OtherActor,UPrimitiveComponent* OtherComp,FVector NormalImpulse,const FHitResult& Hit);

	UFUNCTION(BlueprintCallable,BlueprintNativeEvent)
	void Explode();

	virtual void PostInitializeComponents() override;
public:
	virtual void BeginPlay() override;


};
