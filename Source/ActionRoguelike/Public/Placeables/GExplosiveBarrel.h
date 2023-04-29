// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "PhysicsEngine/RadialForceComponent.h"
#include "GExplosiveBarrel.generated.h"

UCLASS()
class ACTIONROGUELIKE_API AGExplosiveBarrel : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AGExplosiveBarrel();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	UStaticMeshComponent* MeshComp;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	URadialForceComponent* ForceComp;

	UPROPERTY(EditAnywhere)
	float ImpulseDelay;
	

	// implement on-hit event
	void OnActorHit(UPrimitiveComponent* HitComp,AActor* OtherActor,UPrimitiveComponent* OtherComp,FVector NormalImpulse,const FHitResult& Hit);

	// initialize and bind on-hit event to mesh
	virtual void PostInitializeComponents() override;

private:
	
	bool ImpulseLock;
	FTimerHandle ImpulseDelayTimer;
	UFUNCTION()
	void Unlock_ImpulseTime_Elapsed();


};
