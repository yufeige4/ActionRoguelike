// Fill out your copyright notice in the Description page of Project Settings.


#include "Abilities/GAction.h"

#include "GGameplayFunctionLibrary.h"
#include "Components/GActionComponent.h"
#include "Net/UnrealNetwork.h"

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
	// UE_LOG(LogTemp,Log,TEXT("StartAction: %s"),*GetNameSafe(this));
	UGGameplayFunctionLibrary::LogOnScreen(this,FString::Printf(TEXT("Started: %s"),*ActionName.ToString()),FColor::Green);
	ensureAlwaysMsgf(bIsRunning,TEXT("Trying to stop action that was not started!"));
	
	UGActionComponent* ActionComp = GetOwningActionComp();
	if(ActionComp)
	{
		ActionComp->ActiveGameplayTags.AppendTags(GrantedTags);
	}
}

void UGAction::StopAction_Implementation(AActor* Instigator)
{
	// UE_LOG(LogTemp,Log,TEXT("StopAction: %s"),*GetNameSafe(this));
	UGGameplayFunctionLibrary::LogOnScreen(this,FString::Printf(TEXT("Stopped: %s"),*ActionName.ToString()),FColor::Silver);
	UGActionComponent* ActionComp = GetOwningActionComp();
	if(ActionComp)
	{
		ActionComp->ActiveGameplayTags.RemoveTags(GrantedTags);
	}
	bIsRunning = false;
}

UGActionComponent* UGAction::GetOwningActionComp() const
{
	return OwningActionComp;
}

UGAction::UGAction()
{
	bIsRunning = false;
	bAutoStart = false;
}

void UGAction::Initialize(UGActionComponent* NewActionComp)
{
	OwningActionComp = NewActionComp;
}

UWorld* UGAction::GetWorld() const
{
	AActor* Outer = Cast<AActor>(GetOuter());
	if(Outer)
	{
		return Outer->GetWorld();
	}
	return nullptr;
}

bool UGAction::IsRunning() const
{
	return bIsRunning;
}

void UGAction::OnRep_IsRunning()
{
	if(bIsRunning)
	{
		StartAction(nullptr);
	}else
	{
		StopAction(nullptr);
	}
}

void UGAction::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(UGAction,bIsRunning);
	DOREPLIFETIME(UGAction,OwningActionComp);

}