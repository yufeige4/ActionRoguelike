// Fill out your copyright notice in the Description page of Project Settings.


#include "GExplosiveBarrel.h"

#include "GAttributeComponent.h"

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
	// Add which channel of objects to affect
	ForceComp->AddCollisionChannelToAffect(ECC_WorldDynamic);
	ImpulseLock = false;
	ImpulseDelay = 1.0f;
}

// Called when the game starts or when spawned
void AGExplosiveBarrel::BeginPlay()
{
	Super::BeginPlay();
	
}

void AGExplosiveBarrel::OnActorHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp,
	FVector NormalImpulse, const FHitResult& Hit)
{
	if(!ImpulseLock)
	{
		if(OtherActor)
		{
			ImpulseLock = true;
			ForceComp->FireImpulse();
			GetWorldTimerManager().SetTimer(ImpulseDelayTimer,this,&AGExplosiveBarrel::Unlock_ImpulseTime_Elapsed,ImpulseDelay);
			UGAttributeComponent* AttributeComp = Cast<UGAttributeComponent>(OtherActor->GetComponentByClass(UGAttributeComponent::StaticClass()));
			if(AttributeComp)
			{
				AttributeComp->ApplyHealthChange(-50);
			}
			
		}
		// Log this event for check
		UE_LOG(LogTemp, Log, TEXT("OnActorHit reached (ExplosiveBarrel)"));
	}
}

void AGExplosiveBarrel::PostInitializeComponents()
{
	Super::PostInitializeComponents();
	MeshComp->OnComponentHit.AddDynamic(this,&AGExplosiveBarrel::OnActorHit);
}

void AGExplosiveBarrel::Unlock_ImpulseTime_Elapsed()
{
	ImpulseLock = false;
}


