// Fill out your copyright notice in the Description page of Project Settings.


#include "Placeables/GRespawnedPotion.h"

#include "DrawDebugHelpers.h"
#include "Components/GAttributeComponent.h"
#include "Kismet/GameplayStatics.h"

AGRespawnedPotion::AGRespawnedPotion()
{
	HealingAmount = 20.0f;
	
}

void AGRespawnedPotion::Interact_Implementation(APawn* InstigatorPawn)
{
	if(CanInteract)
	{
		UGAttributeComponent* AttributeComp = UGAttributeComponent::GetAttributeComponent(InstigatorPawn);
		if(AttributeComp && !AttributeComp->IsFullHealth())
		{
			UMeshComponent* InstigatorMesh = Cast<UMeshComponent>(InstigatorPawn->GetComponentByClass(UMeshComponent::StaticClass()));
			if(MeshComp)
			{
				UGameplayStatics::SpawnEmitterAttached(HealingEffect,InstigatorMesh,NAME_None,FVector(ForceInit),FRotator::ZeroRotator,FVector(1),EAttachLocation::SnapToTarget);
			}
			UGameplayStatics::PlaySoundAtLocation(GetWorld(),HealingSound,InstigatorPawn->GetActorLocation(),InstigatorPawn->GetActorRotation());
			AttributeComp->ApplyHealthChange(this,HealingAmount);
			
			Super::Interact_Implementation(InstigatorPawn);
		}else
		{
			DrawDebugString(GetWorld(),GetActorLocation(),"Invalid Interact! No AttributeComp or full health!",nullptr,FColor::Red,4,true);
		}
	}
}
