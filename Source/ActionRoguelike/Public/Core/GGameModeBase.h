// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/GSpawnControlComponent.h"
#include "GameFramework/GameModeBase.h"
#include "GGameModeBase.generated.h"

/**
 * 
 */
UCLASS()
class ACTIONROGUELIKE_API AGGameModeBase : public AGameModeBase
{
	GENERATED_BODY()

public:
	AGGameModeBase();

	virtual void StartPlay() override;

protected:

	UPROPERTY(VisibleAnywhere, Category = "Components")
	UGSpawnControlComponent* SpawnControlComp;
	
};
