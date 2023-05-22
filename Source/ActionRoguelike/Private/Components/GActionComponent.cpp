// Fill out your copyright notice in the Description page of Project Settings.


#include "Components/GActionComponent.h"
#include "GGameplayFunctionLibrary.h"
#include "Abilities/GAction.h"
#include "Engine/ActorChannel.h"
#include "Net/UnrealNetwork.h"

static TAutoConsoleVariable<bool> CVarDebugMultiplayerAction(TEXT("ARPG.DebugMultiplayerAction"),false,TEXT("Log multiplayer action info"),ECVF_Cheat);

UGActionComponent::UGActionComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
	SetIsReplicatedByDefault(true);
}

void UGActionComponent::BeginPlay()
{
	Super::BeginPlay();

	// Server中才会存在Action
	if(GetOwner()->HasAuthority())
	{
		for(TSubclassOf<UGAction> Action : DefaultActions)
		{
			// add from default actions should consider owner of comp as Instigator
			AddAction(Action,GetOwner());
		}
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

	if(CVarDebugMultiplayerAction.GetValueOnGameThread())
	{
		for(UGAction* Action : Actions)
		{
			FColor TextColor = Action->IsRunning() ? FColor::Blue : FColor::White;
			FString TextIsRunning = Action->IsRunning() ? TEXT("True") : TEXT("False");
			FString ActionMsg = FString::Printf(TEXT("[%s] Action: %s | IsRunning: %s | Outer: %s"),*GetNameSafe(GetOwner()),*Action->ActionName.ToString(),*TextIsRunning,*GetNameSafe(Action->GetOuter()));

			UGGameplayFunctionLibrary::LogOnScreen(this,ActionMsg,TextColor,0.0f);
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
	UGAction* NewAction =  NewObject<UGAction>(GetOwner(),ActionClass);
	if(ensure(NewAction))
	{
		NewAction->Initialize(this);
		
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
			// after checking conditions, if client then do server RPC
			if(!GetOwner()->HasAuthority())
			{
				ServerStartAction(Instigator,ActionName);
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

void UGActionComponent::ServerStartAction_Implementation(AActor* Instigator, FName ActionName)
{
	StartActionByName(Instigator,ActionName);
}

void UGActionComponent::ServerStopAction_Implementation(AActor* Instigator, FName ActionName)
{
	StopActionByName(Instigator,ActionName);
}

void UGActionComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(UGActionComponent,Actions);
}

bool UGActionComponent::ReplicateSubobjects(UActorChannel* Channel, FOutBunch* Bunch, FReplicationFlags* RepFlags)
{
	bool Output = Super::ReplicateSubobjects(Channel, Bunch, RepFlags);
	for(UGAction* Action : Actions)
	{
		if(Action)
		{
			// 对每个Action进行Replicate
			Output |= Channel->ReplicateSubobject(Action,*Bunch,*RepFlags);
		}
	}
	return Output;
}