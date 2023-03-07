// Fill out your copyright notice in the Description page of Project Settings.


#include "Components/GSpawnControlComponent.h"

#include "EngineUtils.h"
#include "Player/GCharacter.h"
#include "AI/GAICharacter.h"
#include "EnvironmentQuery/EnvQueryManager.h"

// Sets default values for this component's properties
UGSpawnControlComponent::UGSpawnControlComponent()
{
	SpawnInterval = 5.0;
	SpawnNumLimit = 3;
}

void UGSpawnControlComponent::SpawnReset_TimeElapsed()
{
	int32 CurrNumCount = 0;
	for(TActorIterator<AGAICharacter> It(GetWorld()); It; ++It)
	{
		AGAICharacter* AICharacter = *It;
		if(AICharacter)
		{
			auto AttributeComp = UGAttributeComponent::GetAttributeComponent(AICharacter);
			if(AttributeComp && AttributeComp->IsAlive())
			{
				++CurrNumCount;
			}
		}
	}
	int32 NumLimit = SpawnNumLimit;
	if(DifficultyCurve)
	{
		NumLimit = DifficultyCurve->GetFloatValue(GetWorld()->TimeSeconds);
	}
	if(CurrNumCount>=NumLimit)
	{
		return;
	}
	if(ensureAlwaysMsgf(EnvQueryTemplate,TEXT("EnvQueryTemplate need to be select!!!")))
	{
		auto EQSInstance = UEnvQueryManager::RunEQSQuery(GetWorld(),EnvQueryTemplate,GetOwner(),EEnvQueryRunMode::RandomBest5Pct,nullptr);
		if(EQSInstance)
		{
			EQSInstance->GetOnQueryFinishedEvent().AddDynamic(this,&UGSpawnControlComponent::OnQueryFinished);
		}
	}
}

void UGSpawnControlComponent::SpawnObjects()
{
	GetWorld()->GetTimerManager().SetTimer(TimerHandle_SpawnReset,this,&UGSpawnControlComponent::SpawnReset_TimeElapsed,SpawnInterval,true);
}

void UGSpawnControlComponent::StopSpawning()
{
	GetWorld()->GetTimerManager().ClearTimer(TimerHandle_SpawnReset);
}

void UGSpawnControlComponent::OnQueryFinished(UEnvQueryInstanceBlueprintWrapper* QueryInstance,
                                              EEnvQueryStatus::Type QueryStatus)
{
	if(QueryStatus!=EEnvQueryStatus::Success)
	{
		UE_LOG(LogTemp,Warning,TEXT("Spawn Object EQS Failed!"));
		return;
	}
	
	TArray<FVector> Locations = QueryInstance->GetResultsAsLocations();
	if(Locations.Num()>0)
	{
		FActorSpawnParameters SpawnParams;
		SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;
		if(ensureAlwaysMsgf(SpawnClass,TEXT("SpawnClass need to be select!!!")))
		{
			GetWorld()->SpawnActor<AActor>(SpawnClass,Locations[0],FRotator::ZeroRotator,SpawnParams);
		}
	}
}





