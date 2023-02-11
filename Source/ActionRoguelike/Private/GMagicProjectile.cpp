// Fill out your copyright notice in the Description page of Project Settings.


#include "GMagicProjectile.h"

#include "GAttributeComponent.h"
#include "Particles/ParticleSystemComponent.h"

// Sets default values
AGMagicProjectile::AGMagicProjectile()
{
	SphereComp->SetSphereRadius(20.0f);
	SphereComp->OnComponentBeginOverlap.AddDynamic(this,&AGMagicProjectile::OnActorOverlap);
	
	DamageAmount = 20.0f;
}

void AGMagicProjectile::OnActorOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if(OtherActor && OtherActor!=GetInstigator())
	{
		UGAttributeComponent* AttributeComp = Cast<UGAttributeComponent> (OtherActor->GetComponentByClass(UGAttributeComponent::StaticClass()));
		if(AttributeComp)
		{
			AttributeComp->ApplyHealthChange(-DamageAmount);
		}
		Explode();
	}
}
