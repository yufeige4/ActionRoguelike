// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "Components/ActorComponent.h"
#include "GActionComponent.generated.h"


class UGAction;

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class ACTIONROGUELIKE_API UGActionComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	
	UGActionComponent();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Tags")
	FGameplayTagContainer ActiveGameplayTags;

protected:
	
	UPROPERTY()
	TArray<UGAction*> Actions;

	UPROPERTY(EditAnywhere, Category = "Actions")
	TArray<TSubclassOf<UGAction>> DefaultActions;

public:
	
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	UFUNCTION(BlueprintCallable, Category = "Actions")
	UGAction* AddAction(TSubclassOf<UGAction> ActionClass, AActor* Instigator);

	UFUNCTION(BlueprintCallable, Category = "Actions")
	void RemoveAction(UGAction* Action);

	UFUNCTION(BlueprintCallable, Category = "Actions")
	bool StartActionByName(AActor* Instigator, FName ActionName);

	UFUNCTION(BlueprintCallable, Category = "Actions")
	bool StopActionByName(AActor* Instigator, FName ActionName);
	
	UFUNCTION(BlueprintCallable, Category = "Actions")
	static UGActionComponent* GetActionComponent(AActor* Actor);

protected:
	
	virtual void BeginPlay() override;

	UFUNCTION(Server, Reliable)
	void ServerStartAction(AActor* Instigator, FName ActionName);

	UFUNCTION(Server, Reliable)
	void ServerStopAction(AActor* Instigator, FName ActionName);
	
};
