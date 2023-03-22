// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/BTTask_RecoverHealth.h"

#include "AIController.h"
#include "AI/GAICharacterInterface.h"

EBTNodeResult::Type UBTTask_RecoverHealth::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	auto AIC = Cast<AAIController>(OwnerComp.GetOwner());
	if(AIC)
	{
		auto ControlledPawn = AIC->GetPawn();
		if(ControlledPawn && ControlledPawn->Implements<UGAICharacterInterface>())
		{
			IGAICharacterInterface::Execute_Recover(ControlledPawn);
		}
	}
	return Super::ExecuteTask(OwnerComp, NodeMemory);
	
}

