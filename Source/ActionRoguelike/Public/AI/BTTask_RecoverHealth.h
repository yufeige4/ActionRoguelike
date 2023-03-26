// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "BTTask_RecoverHealth.generated.h"

/**
 * 
 */
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnRecoverFinished);

UCLASS()
class ACTIONROGUELIKE_API UBTTask_RecoverHealth : public UBTTaskNode
{
	GENERATED_BODY()

	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;

public:

	UBTTask_RecoverHealth();
	
	FOnRecoverFinished OnRecoverFinished;

protected:

	EBTNodeResult::Type TaskState;
	
	bool bStartTask;

	UFUNCTION()
	void FinishedRecover();
};
