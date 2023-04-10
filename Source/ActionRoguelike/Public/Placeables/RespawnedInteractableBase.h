// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Core/GGameplayInterface.h"
#include "GameFramework/Actor.h"
#include "RespawnedInteractableBase.generated.h"

UENUM()
enum class EInteractType : uint8 {DoNothing, HiddenAfterInteract, DestroyAfterInteract};

// for debugging
#ifndef DEBUG_INTERACTABLE
#define DEBUG_INTERACTABLE
static TAutoConsoleVariable<bool> DebugInteractable(TEXT("ARPG.DebugInteractable"),false,TEXT("Show debug info for Interactables"),ECVF_Cheat);
#endif
UCLASS()
class ACTIONROGUELIKE_API ARespawnedInteractableBase : public AActor, public IGGameplayInterface
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ARespawnedInteractableBase();

protected:

	UPROPERTY(VisibleAnywhere, Category = "Components")
	UStaticMeshComponent* MeshComp;
	
	UPROPERTY(EditAnywhere, Category = "Design")
	float DeltaTimeToRespawn;

	UPROPERTY(EditAnywhere, Category = "Design")
	bool CanInteractAtStart;

	UPROPERTY(EditAnywhere, Category = "Design")
	EInteractType InteractType;

	bool CanInteract;
	
	FTimerHandle TimerHandle_Respawn;
	
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;
	
	virtual void Interact_Implementation(APawn* InstigatorPawn) override;

	virtual void EndInteract_Implementation(APawn* InstigatorPawn) override;

	UFUNCTION(BlueprintCallable, Category = "Internal")
	virtual void Respawn();
};
