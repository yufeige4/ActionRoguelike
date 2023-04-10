// Fill out your copyright notice in the Description page of Project Settings.


#include "Components/GActionComponent.h"
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
		AddAction(Action);
	}
}

void UGActionComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
}

void UGActionComponent::AddAction(TSubclassOf<UGAction> ActionClass)
{
	if(!ensure(ActionClass))
	{
		return;
	}
	// UE创建新的UObject的方式
	UGAction* NewAction =  NewObject<UGAction>(this,ActionClass);
	if(ensure(NewAction))
	{
		Actions.Add(NewAction);
	}
}

bool UGActionComponent::StartActionByName(AActor* Instigator, FName ActionName)
{
	for(UGAction* Action : Actions)
	{
		if(Action && Action->ActionName==ActionName)
		{
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
			Action->StopAction(Instigator);
			return true;
		}
	}
	return false;
}

