// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/GAIController.h"

#include <string>

#include "GGameplayInterface.h"
#include "AI/GAICharacterInterface.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Kismet/GameplayStatics.h"

void AGAIController::BeginPlay()
{
	Super::BeginPlay();
	// remove the previous hard-code logic
	/*APawn* PlayerPawn = UGameplayStatics::GetPlayerPawn(this,0);
	UBlackboardComponent* MyBB = GetBlackboardComponent();
	if(ensure(MyBB)&ensure(PlayerPawn))
	{
		MyBB->SetValueAsObject("TargetActor",PlayerPawn);
	}*/
}

void AGAIController::OnPossess(APawn* InPawn)
{
	Super::OnPossess(InPawn);
	RunBehaviorTree(BehaviorTree);
	
	UBlackboardComponent* MyBB = GetBlackboardComponent();
	

	if(ensure(MyBB))
	{
		if(InPawn->Implements<UGAICharacterInterface>())
		{
			float AttackRange = IGAICharacterInterface::Execute_GetAttackRange(InPawn);
			MyBB->SetValueAsFloat("AttackRange",AttackRange);
		}
	}
}
