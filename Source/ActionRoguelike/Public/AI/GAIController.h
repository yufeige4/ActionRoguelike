// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "GAIController.generated.h"

/**
 * 
 */
class UBehaviorTree;

UCLASS()
class ACTIONROGUELIKE_API AGAIController : public AAIController
{
	GENERATED_BODY()

protected:
	virtual void BeginPlay() override;

	UPROPERTY(EditDefaultsOnly, Category = "AI")
	UBehaviorTree* BehaviorTree;

	virtual void OnPossess(APawn* InPawn) override;
};
