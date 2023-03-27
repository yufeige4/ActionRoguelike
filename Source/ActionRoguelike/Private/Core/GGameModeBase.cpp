// Fill out your copyright notice in the Description page of Project Settings.


#include "Core/GGameModeBase.h"

#include "EngineUtils.h"
#include "AI/GAICharacter.h"
#include "Player/GCharacter.h"


AGGameModeBase::AGGameModeBase()
{
	SpawnControlComp = CreateDefaultSubobject<UGSpawnControlComponent>("SpawnControlComp");
}

void AGGameModeBase::StartPlay()
{
	Super::StartPlay();
	SpawnControlComp->SpawnObjects();
}

void AGGameModeBase::OnActorKilled(AActor* Victim, AActor* Killer)
{
	AGCharacter* Player = Cast<AGCharacter>(Victim);
	if(Player)
	{
		FTimerHandle TimerHandle_RespawnPlayer;
		FTimerDelegate TimerDelegate_RespawnPlayer;
		TimerDelegate_RespawnPlayer.BindUFunction(this,"RespawnTimerElapsed",Player->GetController());
		float RespawnDelay = 5.0f;
		
		GetWorldTimerManager().SetTimer(TimerHandle_RespawnPlayer,TimerDelegate_RespawnPlayer,RespawnDelay,false);
	}
	UE_LOG(LogTemp,Log,TEXT("OnActorKilled: Victim: %s, Killer: %s"),*GetNameSafe(Victim),*GetNameSafe(Killer));
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


