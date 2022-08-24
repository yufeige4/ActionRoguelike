// Fill out your copyright notice in the Description page of Project Settings.


#include "GExplosiveBarrel.h"

// Sets default values
AGExplosiveBarrel::AGExplosiveBarrel()
{
 	MeshComp = CreateDefaultSubobject<UStaticMeshComponent>("MeshComp");
	MeshComp->SetSimulatePhysics(true);
	RootComponent = MeshComp;

	ForceComp = CreateDefaultSubobject<URadialForceComponent>("ForceComp");
	ForceComp->SetupAttachment(MeshComp);
	ForceComp->SetAutoActivate(false);
	// implement radial force params
	ForceComp->Radius = 750.0f;
	ForceComp->ImpulseStrength = 2500.0f;
	// ignore body mass, fixed effect to all
	ForceComp->bImpulseVelChange = true;
	// exclude WorldDynamic
	ForceComp->AddCollisionChannelToAffect(ECC_WorldDynamic);
	
	

}

// Called when the game starts or when spawned
void AGExplosiveBarrel::BeginPlay()
{
	Super::BeginPlay();
	
}

void AGExplosiveBarrel::OnActorHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp,
	FVector NormalImpulse, const FHitResult& Hit)
{
	ForceComp->FireImpulse();
	// Log this event for check
	UE_LOG(LogTemp, Log, TEXT("OnActorHit reached (ExplosiveBarrel)"));
}

void AGExplosiveBarrel::PostInitializeComponents()
{
	Super::PostInitializeComponents();
	MeshComp->OnComponentHit.AddDynamic(this,&AGExplosiveBarrel::OnActorHit);
}
