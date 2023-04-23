// Fill out your copyright notice in the Description page of Project Settings.


#include "Components/GActionComponent.h"
#include "GGameplayFunctionLibrary.h"
#include "Abilities/GAction.h"

UGActionComponent::UGActionComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
}

void UGActionComponent::BeginPlay()
{
	Super::BeginPlay();
	for(TSubclassOf<UGAction> Action : DefaultActions)
	{
		// add from default actions should consider owner of comp as Instigator
		AddAction(Action,GetOwner());
	}
}

void UGActionComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	if(DebugActionTag.GetValueOnGameThread())
	{
		if(GEngine)
		{
			FString DebugMsg = GetNameSafe(GetOwner()) + " : " + ActiveGameplayTags.ToStringSimple();
			GEngine->AddOnScreenDebugMessage(-1,0.0,FColor::White,DebugMsg);
		}
	}
}

UGAction* UGActionComponent::AddAction(TSubclassOf<UGAction> ActionClass, AActor* Instigator)
{
	if(!ensure(ActionClass))
	{
		return nullptr;
	}
	// UE创建新的UObject的方式
	UGAction* NewAction =  NewObject<UGAction>(this,ActionClass);
	if(ensure(NewAction))
	{
		Actions.Add(NewAction);
		if(NewAction->bAutoStart && ensure(NewAction->CanStart(Instigator)))
		{
			NewAction->StartAction(Instigator);
		}
	}
	return NewAction;
}

void UGActionComponent::RemoveAction(UGAction* Action)
{
	if(!ensure(Action && !Action->IsRunning()))
	{
		return;
	}
	Actions.Remove(Action);
}

bool UGActionComponent::StartActionByName(AActor* Instigator, FName ActionName)
{
	for(UGAction* Action : Actions)
	{
		if(Action && Action->ActionName==ActionName)
		{
			if(!Action->CanStart(Instigator))
			{
				return false;
			}
			Action->StartAction(Instigator);
			return true;
		}
	}
	return false;
}

bool UGActionComponent::StopActionByName(AActor* Instigator, FName ActionName)
{
	for(UGAction* Action : Actions)
	{
		if(Action && Action->ActionName==ActionName)
		{
			if(Action->IsRunning())
			{
				Action->StopAction(Instigator);
				return true;
			}
		}
	}
	return false;
}

UGActionComponent* UGActionComponent::GetActionComponent(AActor* Actor)
{
	return Cast<UGActionComponent>(Actor->GetComponentByClass(UGActionComponent::StaticClass()));
}