// Fill out your copyright notice in the Description page of Project Settings.


#include "GGameplayFunctionLibrary.h"

#include "Components/GAttributeComponent.h"

bool UGGameplayFunctionLibrary::ApplyDamage(AActor* FromActor, AActor* ToActor, float DamageAmount)
{
	UGAttributeComponent* AttributeComp = UGAttributeComponent::GetAttributeComponent(ToActor);
	if(AttributeComp)
	{
		return AttributeComp->ApplyHealthChange(FromActor,DamageAmount);
	}
	return false;
}

bool UGGameplayFunctionLibrary::ApplyDirectionalDamage(AActor* FromActor, AActor* ToActor, float DamageAmount,
	const FHitResult& HitResult)
{
	if(ApplyDamage(FromActor,ToActor,DamageAmount))
	{
		auto HitComp = HitResult.GetComponent();
		if(HitComp && HitComp->IsSimulatingPhysics(HitResult.BoneName))
		{
			float ImpulseMagnitude = 100000.0f;
			// FVector ImpulseDirection = -HitResult.ImpactNormal;
			FVector ImpulseDirection = HitResult.TraceEnd-HitResult.TraceStart;
			ImpulseDirection.Normalize();
			HitComp->AddImpulseAtLocation(ImpulseDirection*ImpulseMagnitude,HitResult.ImpactPoint,HitResult.BoneName);
		}
		return true;
	}
	return false;
}

