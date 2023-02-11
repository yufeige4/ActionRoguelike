// Fill out your copyright notice in the Description page of Project Settings.


#include "GAttributeComponent.h"

// Sets default values for this component's properties
UGAttributeComponent::UGAttributeComponent()
{
	Health = 100;
}

bool UGAttributeComponent::ApplyHealthChange(float Delta)
{
	Health += Delta;
	// 调用代理
	OnHealthChanged.Broadcast(nullptr,this,Health,Delta);
	
	return true;
}

