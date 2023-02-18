// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GAttributeComponent.h"
#include "GameFramework/Actor.h"
#include "GHitFlashingTest.generated.h"

UCLASS()
class ACTIONROGUELIKE_API AGHitFlashingTest : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AGHitFlashingTest();

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	UGAttributeComponent* AttributeComp;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	UStaticMeshComponent* MeshComp;
	
	UFUNCTION()
	void HitFlash(AActor* InstigatorActor, UGAttributeComponent* OwningComp, float NewHealth, float Delta);

};
