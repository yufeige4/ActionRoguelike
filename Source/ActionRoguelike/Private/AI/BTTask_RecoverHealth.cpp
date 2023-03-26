// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/BTTask_RecoverHealth.h"

#include "AIController.h"
#include "AI/GAICharacterInterface.h"

EBTNodeResult::Type UBTTask_RecoverHealth::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	if(!bStartTask)
	{
		auto AIC = Cast<AAIController>(OwnerComp.GetOwner());
		if(AIC)
		{
			auto ControlledPawn = AIC->GetPawn();
			if(IsValid(ControlledPawn) && ControlledPawn->Implements<UGAICharacterInterface>())
			{
				IGAICharacterInterface::Execute_Recover(ControlledPawn,this);
				TaskState = EBTNodeResult::InProgress;
				bStartTask = true;
			}
		}
	}
	return TaskState;
}

UBTTask_RecoverHealth::UBTTask_RecoverHealth()
{
	bStartTask = false;
	TaskState = EBTNodeResult::Succeeded;
	OnRecoverFinished.AddDynamic(this,&UBTTask_RecoverHealth::FinishedRecover);
}

void UBTTask_RecoverHealth::FinishedRecover()
{
	UE_LOG(LogTemp,Log,TEXT("Finish Recover!!!"));
	bStartTask = false;
	TaskState = EBTNodeResult::Succeeded;
}

