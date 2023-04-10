// Fill out your copyright notice in the Description page of Project Settings.


#include "Abilities/GAction.h"

#include "Components/GActionComponent.h"

void UGAction::StartAction_Implementation(AActor* Instigator)
{
	UE_LOG(LogTemp,Log,TEXT("StartAction: %s"),*GetNameSafe(this));
}

void UGAction::StopAction_Implementation(AActor* Instigator)
{
	UE_LOG(LogTemp,Log,TEXT("StopAction: %s"),*GetNameSafe(this));
}

UWorld* UGAction::GetWorld() const
{
	UGActionComponent* ActionComp = Cast<UGActionComponent>(GetOuter());
	if(ActionComp)
	{
		return ActionComp->GetWorld();
	}
	return nullptr;
}
