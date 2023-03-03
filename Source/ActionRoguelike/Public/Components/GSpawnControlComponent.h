// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "EnvironmentQuery/EnvQuery.h"
#include "GSpawnControlComponent.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class ACTIONROGUELIKE_API UGSpawnControlComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UGSpawnControlComponent();

protected:

	UPROPERTY(EditDefaultsOnly, Category = "AI")
	UCurveFloat* DifficultyCurve;

	UPROPERTY(EditDefaultsOnly, Category = "AI")
	int32 SpawnNumLimit;
	
	UPROPERTY(EditDefaultsOnly, Category = "AI")
	float SpawnInterval;

	UPROPERTY(EditDefaultsOnly, Category = "AI")
	TSubclassOf<AActor> SpawnClass;

	UPROPERTY(EditDefaultsOnly, Category = "AI")
	UEnvQuery* EnvQueryTemplate;

	FTimerHandle TimerHandle_SpawnReset;
	
public:
	UFUNCTION(BlueprintCallable, Category = "AI")
	void SpawnObjects();

	UFUNCTION(BlueprintCallable, Category = "AI")
	void StopSpawning();

protected:
	UFUNCTION()
	void SpawnReset_TimeElapsed();

	UFUNCTION()
	void OnQueryFinished(UEnvQueryInstanceBlueprintWrapper* QueryInstance, EEnvQueryStatus::Type QueryStatus);

};
