// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTService.h"
#include "GBTService_HealthCheck.generated.h"

/**
 * 
 */
UCLASS()
class ACTIONROGUELIKE_API UGBTService_HealthCheck : public UBTService
{
	GENERATED_BODY()
protected:
	
	virtual void TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds) override;

	UPROPERTY(EditAnywhere, Category = "AI")
	float EscapeRatio;

	UPROPERTY(EditAnywhere, Category = "AI")
	FBlackboardKeySelector Key_AIState;

	bool bHaveTriggered = false;
};
