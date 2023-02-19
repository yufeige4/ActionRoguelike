// Fill out your copyright notice in the Description page of Project Settings.


#include "GAttributeComponent.h"

// Sets default values for this component's properties
UGAttributeComponent::UGAttributeComponent()
{
	CurrHealth = 100.0f;
	MaxHealth = 100.0f;
}

bool UGAttributeComponent::ApplyHealthChange(float Delta)
{
	CurrHealth = FMath::Clamp(CurrHealth+Delta,0.0f,MaxHealth);
	// 调用代理
	OnHealthChanged.Broadcast(nullptr,this,CurrHealth,Delta);
	
	return true;
}

bool UGAttributeComponent::IsAlive() const
{
	return CurrHealth > 0.0f;
}

