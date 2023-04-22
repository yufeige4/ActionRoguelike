// Fill out your copyright notice in the Description page of Project Settings.


#include "Abilities/GAction.h"
#include "Components/GActionComponent.h"

bool UGAction::CanStart_Implementation(AActor* Instigator)
{
	if(IsRunning())
	{
		return false;
	}
	UGActionComponent* ActionComp = GetOwningActionComp();
	if(ActionComp && ActionComp->ActiveGameplayTags.HasAny(BlockedTags))
	{
		return false;
	}
	bIsRunning = true;
	return true;
}

void UGAction::StartAction_Implementation(AActor* Instigator)
{
	UE_LOG(LogTemp,Log,TEXT("StartAction: %s"),*GetNameSafe(this));

	ensureAlwaysMsgf(bIsRunning,TEXT("Trying to stop action that was not started!"));
	
	UGActionComponent* ActionComp = GetOwningActionComp();
	if(ActionComp)
	{
		ActionComp->ActiveGameplayTags.AppendTags(GrantedTags);
	}
}

void UGAction::StopAction_Implementation(AActor* Instigator)
{
	UE_LOG(LogTemp,Log,TEXT("StopAction: %s"),*GetNameSafe(this));
	
	UGActionComponent* ActionComp = GetOwningActionComp();
	if(ActionComp)
	{
		ActionComp->ActiveGameplayTags.RemoveTags(GrantedTags);
	}
	bIsRunning = false;
}

UGActionComponent* UGAction::GetOwningActionComp() const
{
	return Cast<UGActionComponent>(GetOuter());
}

UGAction::UGAction()
{
	bIsRunning = false;
	bAutoStart = false;
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

bool UGAction::IsRunning() const
{
	return bIsRunning;
}