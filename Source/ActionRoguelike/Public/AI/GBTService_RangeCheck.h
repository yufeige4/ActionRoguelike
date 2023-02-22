// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTService.h"
#include "GBTService_RangeCheck.generated.h"

/**
 * 
 */
UCLASS()
class ACTIONROGUELIKE_API UGBTService_RangeCheck : public UBTService
{
	GENERATED_BODY()

protected:
	virtual void TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds) override;

	UPROPERTY(EditAnywhere, Category = "AI")
	FBlackboardKeySelector Key_TargetActor;
	
	UPROPERTY(EditAnywhere, Category = "AI")
	FBlackboardKeySelector Key_AttackRange;

	UPROPERTY(EditAnywhere, Category = "AI")
	FBlackboardKeySelector Key_ChasingTarget
	;
	
};
