// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "GEventManager.generated.h"

// 代理声明
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnKillActor, AActor*, Killer, AActor*, Victim);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnUseItem, AActor*, User, AActor*, Item);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnPickUpItem, AActor*, Picker, AActor*, Item);

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class ACTIONROGUELIKE_API UGEventManager : public UActorComponent
{
	GENERATED_BODY()

public:
	
	UGEventManager();

	UFUNCTION(BlueprintCallable, Category = "Event")
	static UGEventManager* GetEventManager(AActor* Actor);


protected:

public:
	
	UFUNCTION(BlueprintCallable, Category = "Event")
	virtual void OnActorKilled(AActor* Victim, AActor* Killer);

	UPROPERTY(BlueprintAssignable, Category = "Event")
	FOnKillActor OnKillActor;

	UFUNCTION(BlueprintCallable, Category = "Event")
	virtual void OnItemUsed(AActor* User, AActor* Item);

	UPROPERTY(BlueprintAssignable, Category = "Event")
	FOnUseItem OnUseItem;

	UFUNCTION(BlueprintCallable, Category = "Event")
	virtual void OnItemPickedUp(AActor* Picker, AActor* Item);

	UPROPERTY(BlueprintAssignable, Category = "Event")
	FOnPickUpItem OnPickUpItem;

};
