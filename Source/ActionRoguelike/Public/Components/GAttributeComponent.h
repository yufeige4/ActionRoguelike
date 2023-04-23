// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "GAttributeComponent.generated.h"

// 代理声明
DECLARE_DYNAMIC_MULTICAST_DELEGATE_FourParams(FOnHealthChanged, AActor*, InstigatorActor, UGAttributeComponent*, OwningComp, float, NewHealth, float, Delta);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_FiveParams(FOnRageChanged, AActor*, InstigatorActor, UGAttributeComponent*, OwningComp, int32, NewRage, int32, Delta, bool, IsSuccess);

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class ACTIONROGUELIKE_API UGAttributeComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UGAttributeComponent();

protected:
	// 当前生命值
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Attributes")
	float CurrHealth;
	// 最大生命值
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Attributes")
	float MaxHealth;
	// 当前怒气
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Attributes")
	int32 CurrRage;
	// 愤怒值变化量
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Attributes")
	int32 RageGrowthRate;
	// 最大怒气
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Attributes")
	int32 MaxRage;
	
public:
	// 判断怒气值是否足够
	UFUNCTION(BlueprintCallable, Category = "Attributes")
	bool HaveEnoughRage(int32 RageCost);
	// 对怒气值进行修改
	UFUNCTION(BlueprintCallable, Category = "Attributes")
	bool ApplyRageChange(AActor* Instigator, int32 Delta);
	// 包含代理
	UPROPERTY(BlueprintAssignable, Category = "Attributes")
	FOnRageChanged OnRageChanged;
	// 对生命值进行修改
	UFUNCTION(BlueprintCallable, Category = "Attributes")
	bool ApplyHealthChange(AActor* Instigator, float Delta);
	// 包含代理
	UPROPERTY(BlueprintAssignable, Category = "Attributes")
	FOnHealthChanged OnHealthChanged;

	UFUNCTION(BlueprintCallable, Category = "Attributes")
	bool IsAlive() const;

	UFUNCTION(BlueprintCallable, Category = "Attributes")
	bool IsFullHealth() const;

	UFUNCTION(BlueprintCallable, Category = "Attributes")
	static UGAttributeComponent* GetAttributeComponent(AActor* Actor);
	
	UFUNCTION(BlueprintCallable, Category = "Attributes", meta = (DisplayName = "IsAlive"))
	static bool IsActorAlive(AActor* Actor);

	UFUNCTION(BlueprintCallable, Category = "Attributes")
	float GetCurrHealth();

	UFUNCTION(BlueprintCallable, Category = "Attributes")
	float GetMaxHealth();

	UFUNCTION(BlueprintCallable, Category = "Attributes")
	float GetMaxRage();
	
	UFUNCTION(BlueprintCallable, Category = "Attributes")
	bool Kill(AActor* Instigator);
};
