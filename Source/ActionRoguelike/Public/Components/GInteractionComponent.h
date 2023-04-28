// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "UI/GUserWidget_World.h"
#include "GInteractionComponent.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class ACTIONROGUELIKE_API UGInteractionComponent : public UActorComponent
{
	GENERATED_BODY()

protected:

	UPROPERTY(EditDefaultsOnly, Category = "Trace")
	float TraceDistance;

	UPROPERTY(EditDefaultsOnly, Category = "Trace")
	float TraceRadius;

	UPROPERTY(EditDefaultsOnly, Category = "Trace")
	TEnumAsByte<ECollisionChannel> CollisionChannel;
	
	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = "Interact")
	AActor* FocusedActor;

	UPROPERTY(EditDefaultsOnly, Category = "UI")
	TSubclassOf<UGUserWidget_World> HintWidgetClass;

	UPROPERTY()
	UGUserWidget_World* HintWidgetInstance;

	APawn* OwningPawn;

public:	
	// Sets default values for this component's properties
	UGInteractionComponent();
	
	void PrimaryInteract();

protected:
	
	UFUNCTION(Server, Unreliable)
	void ServerInteract(AActor* InFocusedActor);
	
	// Called when the game starts
	virtual void BeginPlay() override;

	void FindBestInteractable();

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
		
};
