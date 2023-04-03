// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerState.h"
#include "GPlayerStateBase.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams(FOnCreditChange, int, ActualDelta, int, PrevCredit, int, NewCredit);
/**
 * 
 */
UCLASS()
class ACTIONROGUELIKE_API AGPlayerStateBase : public APlayerState
{
	GENERATED_BODY()

protected:

	// Better implement credit in credit system
	UPROPERTY(BlueprintReadOnly)
	int Credit;

public:
	
	AGPlayerStateBase();

	UFUNCTION(BlueprintCallable, Category = "Credit")
	bool ApplyCreditChange(int Delta);

	UPROPERTY(BlueprintAssignable, Category = "Credit")
	FOnCreditChange OnCreditChange;

protected:

	virtual void PostInitializeComponents() override;

	UFUNCTION()
	void CreditChangeOnKillEvent(AActor* Victim,AActor* Killer);

	UFUNCTION()
	void CreditChangeOnUseItemEvent(AActor* User,AActor* Item);

	UFUNCTION()
	void CreditChangeOnPickUpItemEvent(AActor* Picker,AActor* Item);
};
