// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GAction.generated.h"

class UWorld;
/**
 * 
 */
UCLASS(Blueprintable)
class ACTIONROGUELIKE_API UGAction : public UObject
{
	GENERATED_BODY()

public:
	// nickname of an action
	UPROPERTY(EditDefaultsOnly, Category = "Action")
	FName ActionName;

	UFUNCTION(BlueprintNativeEvent, Category = "Action")
	void StartAction(AActor* Instigator);

	UFUNCTION(BlueprintNativeEvent, Category = "Action")
	void StopAction(AActor* Instigator);

	virtual UWorld* GetWorld() const override;
	
};
