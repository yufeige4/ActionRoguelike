// Fill out your copyright notice in the Description page of Project Settings.


#include "GRespawnedPotion.h"

#include "GAttributeComponent.h"
#include "Kismet/GameplayStatics.h"

AGRespawnedPotion::AGRespawnedPotion()
{
	HealingAmount = 20.0f;
	
}

void AGRespawnedPotion::Interact_Implementation(APawn* InstigatorPawn)
{
	if(CanInteract)
	{
		UGAttributeComponent* AttributeComp = Cast<UGAttributeComponent>(InstigatorPawn->GetComponentByClass(UGAttributeComponent::StaticClass()));
		if(AttributeComp)
		{
			UMeshComponent* InstigatorMesh = Cast<UMeshComponent>(InstigatorPawn->GetComponentByClass(UMeshComponent::StaticClass()));
			if(MeshComp)
			{
				UGameplayStatics::SpawnEmitterAttached(HealingEffect,InstigatorMesh,NAME_None,FVector(ForceInit),FRotator::ZeroRotator,FVector(1),EAttachLocation::SnapToTarget);
			}
			UGameplayStatics::PlaySoundAtLocation(GetWorld(),HealingSound,InstigatorPawn->GetActorLocation(),InstigatorPawn->GetActorRotation());
			AttributeComp->ApplyHealthChange(HealingAmount);
		}
	}
	Super::Interact_Implementation(InstigatorPawn);
}
