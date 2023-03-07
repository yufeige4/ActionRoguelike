// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/BTTask_AttackTarget.h"

#include "AIController.h"
#include "AI/GAICharacterInterface.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Components/GAttributeComponent.h"

EBTNodeResult::Type UBTTask_AttackTarget::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	AAIController* MyController = OwnerComp.GetAIOwner();
	UBlackboardComponent* MyBB = OwnerComp.GetBlackboardComponent();

	if(ensure(MyController)&ensure(MyBB))
	{
		APawn* MyPawn = MyController->GetPawn();
		if(MyPawn && MyPawn->Implements<UGAICharacterInterface>())
		{
			AActor* TargetActor = Cast<AActor>(MyBB->GetValueAsObject(Key_TargetActor.SelectedKeyName));
			if(TargetActor && UGAttributeComponent::IsActorAlive(TargetActor))
			{
				IGAICharacterInterface::Execute_Attack(MyPawn,TargetActor);
				return EBTNodeResult::Succeeded;
			}
		}
		
	}
	return EBTNodeResult::Failed;
}
