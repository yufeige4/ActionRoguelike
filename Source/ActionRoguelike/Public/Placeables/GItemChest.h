// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Core/GGameplayInterface.h"
#include "GameFramework/Actor.h"
#include "GItemChest.generated.h"

UCLASS()
class ACTIONROGUELIKE_API AGItemChest : public AActor, public IGGameplayInterface
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere)
	float TargetRoll;
	
	void Interact_Implementation(APawn* InstigatorPawn);
	

	// Sets default values for this actor's properties
	AGItemChest();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UPROPERTY(VisibleAnywhere)
	UStaticMeshComponent* BaseMesh;
	
	UPROPERTY(VisibleAnywhere,BlueprintReadOnly)
	UStaticMeshComponent* LidMesh;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

};
