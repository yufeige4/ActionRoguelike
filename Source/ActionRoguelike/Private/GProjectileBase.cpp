// Fill out your copyright notice in the Description page of Project Settings.


#include "GProjectileBase.h"

#include "Components/AudioComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Particles/ParticleSystemComponent.h"

// Sets default values
AGProjectileBase::AGProjectileBase()
{
	// Set up SphereComp to RootComp and collision type
	SphereComp = CreateDefaultSubobject<USphereComponent>("SphereComp");
	SphereComp->SetCollisionProfileName("Projectile");
	
	// Set up EffectComp
	EffectComp = CreateDefaultSubobject<UParticleSystemComponent>("EffectComp");
	EffectComp->SetupAttachment(SphereComp);

	// set up Movement, its initial speed and other params
	MoveComp = CreateDefaultSubobject<UProjectileMovementComponent>("MovementComp");
	MoveComp->InitialSpeed = ProjectileSpeed;
	MoveComp->bRotationFollowsVelocity = true;
	MoveComp->bInitialVelocityInLocalSpace = true;
	MoveComp->ProjectileGravityScale = 0.0f;

	// Set up AudioComp
	AudioComp = CreateDefaultSubobject<UAudioComponent>("AudioComp");
}


void AGProjectileBase::OnActorHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp,
	FVector NormalImpulse, const FHitResult& Hit)
{
	Explode();
}

void AGProjectileBase::Explode_Implementation()
{
	if(ensure(!IsPendingKill()))
	{
		// 实现了之前蓝图中实现的爆破效果
		UGameplayStatics::SpawnEmitterAtLocation(this,ImpactVFX,GetActorLocation(),GetActorRotation());
		EffectComp->DeactivateSystem();
		
		MoveComp->StopMovementImmediately();
		SetActorEnableCollision(false);

		Destroy();
	}
}

void AGProjectileBase::PostInitializeComponents()
{
	Super::PostInitializeComponents();
	// 弹道移动中忽略触发者本身
	SphereComp->IgnoreActorWhenMoving(GetInstigator(),true);
	// 绑定Hit事件
	SphereComp->OnComponentHit.AddDynamic(this,&AGProjectileBase::OnActorHit);
}

void AGProjectileBase::BeginPlay()
{
	Super::BeginPlay();
	AudioComp->Play();
}


