// Fill out your copyright notice in the Description page of Project Settings.


#include "Core/GGameModeBase.h"
#include "EngineUtils.h"
#include "AI/GAICharacter.h"
#include "Components/GEventManager.h"
#include "Components/GSpawnControlComponent.h"
#include "Player/GCharacter.h"

AGGameModeBase::AGGameModeBase()
{
	SpawnControlComp = CreateDefaultSubobject<UGSpawnControlComponent>("SpawnControlComp");
	EventManager = CreateDefaultSubobject<UGEventManager>("EventManager");
}

void AGGameModeBase::StartPlay()
{
	Super::StartPlay();
	SpawnControlComp->SpawnObjects();
}

void AGGameModeBase::KillAll()
{
	for(TActorIterator<AGAICharacter> It(GetWorld()); It; ++It)
	{
		AGAICharacter* AICharacter = *It;
		if(AICharacter)
		{
			auto AttributeComp = UGAttributeComponent::GetAttributeComponent(AICharacter);
			if(ensure(AttributeComp) && AttributeComp->IsAlive())
			{
				AttributeComp->Kill(this); // @fixme: Maybe pass in player for credits
			}
		}
	}
}

void AGGameModeBase::RespawnTimerElapsed(APlayerController* PC)
{
	if(ensure(PC))
	{
		PC->UnPossess();
		RestartPlayer(PC);
	}
}


