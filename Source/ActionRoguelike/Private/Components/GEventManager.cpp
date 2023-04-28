// Fill out your copyright notice in the Description page of Project Settings.


#include "Components/GEventManager.h"
#include "Player/GCharacter.h"
#include "EngineUtils.h"
#include "Core/GGameModeBase.h"

static TAutoConsoleVariable<bool> ShowEventLog(TEXT("ARPG.ShowEventLog"),false,TEXT("Display Event Log"),ECVF_Cheat);

// Sets default values for this component's properties
UGEventManager::UGEventManager()
{
	
}

UGEventManager* UGEventManager::GetEventManager(AActor* Actor)
{
	if(!Actor)
	{
		return nullptr;
	}
	auto GM = Actor->GetWorld()->GetAuthGameMode<AGGameModeBase>();
	if(GM)
	{
		UGEventManager* EventManager = Cast<UGEventManager>(GM->GetComponentByClass(UGEventManager::StaticClass()));
		return EventManager;
	}
	return nullptr;
}


void UGEventManager::OnActorKilled(AActor* Victim, AActor* Killer)
{

	OnKillActor.Broadcast(Victim,Killer);
	
	if(ShowEventLog.GetValueOnGameThread())
	{
		UE_LOG(LogTemp,Log,TEXT("OnActorKilled: Victim: %s, Killer: %s"),*GetNameSafe(Victim),*GetNameSafe(Killer));
	}
}

void UGEventManager::OnItemUsed(AActor* User, AActor* Item)
{
	OnUseItem.Broadcast(User,Item);
	
	if(ShowEventLog.GetValueOnGameThread())
	{
		UE_LOG(LogTemp,Log,TEXT("%s Used Item %s"),*GetNameSafe(User),*GetNameSafe(Item));
	}
}

void UGEventManager::OnItemPickedUp(AActor* Picker, AActor* Item)
{
	OnPickUpItem.Broadcast(Picker,Item);

	if(ShowEventLog.GetValueOnGameThread())
	{
		UE_LOG(LogTemp,Log,TEXT("%s pick up Item %s"),*GetNameSafe(Picker),*GetNameSafe(Item));
	}
}

