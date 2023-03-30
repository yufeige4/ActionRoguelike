// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "GGameplayFunctionLibrary.generated.h"

/**
 * 
 */
UENUM(BlueprintType)
enum class EAIState : uint8
{
	InCombat	UMETA(DisplayName = "战斗状态"),
	OutCombat	UMETA(DisplayName = "脱战状态"),
	Died	UMETA(DisplayName = "死亡")
};

UCLASS()
class ACTIONROGUELIKE_API UGGameplayFunctionLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, Category = "Gameplay")
	static bool ApplyDamage(AActor* FromActor, AActor* ToActor, float DamageAmount);

	UFUNCTION(BlueprintCallable, Category = "Gameplay")
	static bool ApplyDirectionalDamage(AActor* FromActor, AActor* ToActor, float DamageAmount, const FHitResult& HitResult);
};