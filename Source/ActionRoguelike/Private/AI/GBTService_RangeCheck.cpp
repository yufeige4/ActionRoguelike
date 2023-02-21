// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/GBTService_RangeCheck.h"

#include "AIController.h"
#include "BehaviorTree/BlackboardComponent.h"

void UGBTService_RangeCheck::TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	Super::TickNode(OwnerComp, NodeMemory, DeltaSeconds);

	UBlackboardComponent* MyBB = OwnerComp.GetBlackboardComponent();
	AAIController* MyController = OwnerComp.GetAIOwner();
	
	if(ensure(MyBB)&&ensure(MyController))
	{
		APawn* MyPawn = MyController->GetPawn();
		AActor* TargetActor = Cast<AActor>(MyBB->GetValueAsObject(Key_TargetActor.SelectedKeyName));
		if(ensure(MyPawn))
		{
			if(TargetActor)
			{
				float AttackRange = MyBB->GetValueAsFloat(Key_AttackRange.SelectedKeyName);
				float Distance = FVector::Distance(TargetActor->GetActorLocation(),MyPawn->GetActorLocation());

				bool InsideAttackRange = Distance <= AttackRange;
				MyBB->SetValueAsBool(Key_InsideAttackRange.SelectedKeyName,InsideAttackRange);
			}
		}
	}
}
