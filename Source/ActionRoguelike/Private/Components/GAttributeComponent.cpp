// Fill out your copyright notice in the Description page of Project Settings.


#include "Components/GAttributeComponent.h"

#include "Components/GEventManager.h"
#include "Core/GGameModeBase.h"
#include "Net/UnrealNetwork.h"

static TAutoConsoleVariable<float> CVarDamageMultiplier(TEXT("ARPG.DamageMultiplier"),1.0f,TEXT("Global ratio of damage dealing"),ECVF_Cheat);
// Sets default values for this component's properties
UGAttributeComponent::UGAttributeComponent()
{
	CurrHealth = 100.0f;
	MaxHealth = 100.0f;
	CurrRage = 0;
	RageGrowthRate = 5;
	MaxRage = 100;

	SetIsReplicatedByDefault(true);
}

bool UGAttributeComponent::HaveEnoughRage(int32 RageCost)
{
	return (CurrRage-RageCost)>=0;
}

bool UGAttributeComponent::ApplyRageChange(AActor* Instigator, int32 Delta)
{
	if(Delta==0)
	{
		return false;
	}
	int32 NewRage = FMath::Clamp(CurrRage+Delta,0,MaxRage);
	int32 ActualDelta = NewRage-CurrRage;
	bool bSuccess = true;
	if(Delta<0)
	{
		bSuccess = Delta==ActualDelta ? true : false;
	}
	if(bSuccess)
	{
		CurrRage = NewRage;
	}
	OnRageChanged.Broadcast(Instigator,this,CurrRage,ActualDelta,bSuccess);
	return bSuccess;
}

bool UGAttributeComponent::ApplyHealthChange(AActor* Instigator, float Delta)
{
	if(!GetOwner()->CanBeDamaged() && Delta<0.0f)
	{
		return false;		
	}
	
	float Ratio = 1;
	if(Delta<0.0f)
	{
		Ratio = CVarDamageMultiplier.GetValueOnGameThread();
		Delta *= Ratio;
	}
	
	float PreviousHealth = CurrHealth;
	CurrHealth = FMath::Clamp(CurrHealth+Delta,0.0f,MaxHealth);
	float AcutualDelta = CurrHealth - PreviousHealth;

	if(AcutualDelta<0.0f)
	{
		ApplyRageChange(Instigator,RageGrowthRate);
	}
	
	// 调用代理
	// OnHealthChanged.Broadcast(Instigator,this,CurrHealth,AcutualDelta);
	if(AcutualDelta!=0.0f)
	{
		MulticastHealthChanged(Instigator,CurrHealth,AcutualDelta);
	}
	// Died
	if(AcutualDelta<0.0f && CurrHealth==0.0f)
	{
		auto Owner = GetOwner();
		auto EventManager = UGEventManager::GetEventManager(Owner);
		if(EventManager)
		{
			EventManager->OnActorKilled(Owner,Instigator);
		}
	}
	return true;
}

bool UGAttributeComponent::IsAlive() const
{
	return CurrHealth > 0.0f;
}

bool UGAttributeComponent::IsFullHealth() const
{
	return CurrHealth==MaxHealth;
}

UGAttributeComponent* UGAttributeComponent::GetAttributeComponent(AActor* Actor)
{
	if(Actor)
	{
		return Cast<UGAttributeComponent>(Actor->GetComponentByClass(UGAttributeComponent::StaticClass()));
	}
	return nullptr;
}

// return false if no AttributeComp or not alive
bool UGAttributeComponent::IsActorAlive(AActor* Actor)
{
	UGAttributeComponent* AttributeComp = GetAttributeComponent(Actor);
	return (AttributeComp && AttributeComp->IsAlive());
}

float UGAttributeComponent::GetCurrHealth()
{
	return CurrHealth;
}

float UGAttributeComponent::GetMaxHealth()
{
	return MaxHealth;
}

float UGAttributeComponent::GetMaxRage()
{
	return MaxRage;
}

bool UGAttributeComponent::Kill(AActor* Instigator)
{
	return ApplyHealthChange(Instigator,-MaxHealth);
}

void UGAttributeComponent::MulticastHealthChanged_Implementation(AActor* Instigator, float NewHealth, float Delta)
{
	OnHealthChanged.Broadcast(Instigator,this,NewHealth,Delta);
}

void UGAttributeComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(UGAttributeComponent,CurrHealth);
	DOREPLIFETIME(UGAttributeComponent,MaxHealth);
	// if MaxHealth is changed, only the owner can see it, for saving bandwidth
	// DOREPLIFETIME_CONDITION(UGAttributeComponent,CurrHealth,COND_OwnerOnly);
}
