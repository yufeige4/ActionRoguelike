// Fill out your copyright notice in the Description page of Project Settings.


#include "Components/GAttributeComponent.h"

// Sets default values for this component's properties
UGAttributeComponent::UGAttributeComponent()
{
	CurrHealth = 100.0f;
	MaxHealth = 100.0f;
}

bool UGAttributeComponent::ApplyHealthChange(AActor* Instigator, float Delta)
{
	float PreviousHealth = CurrHealth;
	CurrHealth = FMath::Clamp(CurrHealth+Delta,0.0f,MaxHealth);
	float AcutualDelta = CurrHealth - PreviousHealth;
	// 调用代理
	OnHealthChanged.Broadcast(Instigator,this,CurrHealth,AcutualDelta);
	
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

