// Fill out your copyright notice in the Description page of Project Settings.


#include "Placeables/GHitFlashingTest.h"

AGHitFlashingTest::AGHitFlashingTest()
{
	MeshComp = CreateDefaultSubobject<UStaticMeshComponent>("MeshComp");
	RootComponent = MeshComp;
	AttributeComp = CreateDefaultSubobject<UGAttributeComponent>("AttributeComp");

	AttributeComp->OnHealthChanged.AddDynamic(this,&AGHitFlashingTest::HitFlash);
}

void AGHitFlashingTest::HitFlash(AActor* InstigatorActor, UGAttributeComponent* OwningComp, float NewHealth,
	float Delta) 
{
	if(Delta<0.0f)
	{
		MeshComp->SetScalarParameterValueOnMaterials("TimeToHit",GetWorld()->TimeSeconds);
	}
}



