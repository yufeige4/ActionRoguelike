// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "GGameModeBase.generated.h"

class UGEventManager;
class UGSpawnControlComponent;
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

	UPROPERTY(VisibleAnywhere, Category = "Components")
	UGEventManager* EventManager;

public:
	
	UFUNCTION(Exec)
	void KillAll();

protected:

	virtual void PostInitializeComponents() override;

	UFUNCTION()
	void OnActorKilled(AActor* Victim, AActor* Killer);
	
	UFUNCTION()
	void RespawnTimerElapsed(APlayerController* PC);
	
};
