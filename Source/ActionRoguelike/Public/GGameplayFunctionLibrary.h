// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "GGameplayFunctionLibrary.generated.h"

/**
 * 
 */
#ifndef DEBUG_ACTION_TAG
#define DEBUG_ACTION_TAG
static TAutoConsoleVariable<bool> DebugActionTag(TEXT("ARPG.ShowActionTag"),false,TEXT("show action tag debug message on screen"),ECVF_Cheat);
#endif

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

	static void LogOnScreen(UObject* WorldContent, FString Msg, FColor Color = FColor::White, float Duration = 5.0f);
};
