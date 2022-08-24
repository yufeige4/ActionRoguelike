// Fill out your copyright notice in the Description page of Project Settings.


#include "GMagicProjectile.h"
#include "Particles/ParticleSystemComponent.h"

// Sets default values
AGMagicProjectile::AGMagicProjectile()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	// Set up SphereComp to RootComp and collision type
	SphereComp = CreateDefaultSubobject<USphereComponent>("SphereComp");
	SphereComp->SetCollisionProfileName("Projectile");
	RootComponent = SphereComp;
	
	// Set up EffectComp
	EffectComp = CreateDefaultSubobject<UParticleSystemComponent>("EffectComp");
	EffectComp->SetupAttachment(SphereComp);

	// set up Movement, its initial speed and other params
	MovementComp = CreateDefaultSubobject<UProjectileMovementComponent>("MovementComp");
	MovementComp->InitialSpeed = 1000.0f;
	MovementComp->bRotationFollowsVelocity = true;
	MovementComp->bInitialVelocityInLocalSpace = true;

}

// Called when the game starts or when spawned
void AGMagicProjectile::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AGMagicProjectile::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

