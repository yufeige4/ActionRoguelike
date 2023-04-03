// Fill out your copyright notice in the Description page of Project Settings.


#include "Placeables/GRespawnedCreditBonus.h"

#include "Components/GEventManager.h"

void AGRespawnedCreditBonus::Interact_Implementation(APawn* InstigatorPawn)
{
	// Broadcast Event
	auto EventManger = UGEventManager::GetEventManager(this);
	if(EventManger)
	{
		EventManger->OnItemPickedUp(InstigatorPawn,this);
	}
	Super::Interact_Implementation(InstigatorPawn);
}
