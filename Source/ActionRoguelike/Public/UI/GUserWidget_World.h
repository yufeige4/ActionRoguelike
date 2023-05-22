// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "GUserWidget_World.generated.h"

class USizeBox;
/**
 * 
 */
UCLASS()
class ACTIONROGUELIKE_API UGUserWidget_World : public UUserWidget
{
	GENERATED_BODY()

protected:
	
	UPROPERTY(meta=(BindWidget))
	USizeBox* ParentSizeBox;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "UI", meta = (ExposeOnSpawn=true))
	FVector WorldDisplayOffset;
	
public:
	
	UPROPERTY(BlueprintReadOnly, Category = "UI", meta = (ExposeOnSpawn=true))
	AActor* AttachedActor;

protected:
	
	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;
};
