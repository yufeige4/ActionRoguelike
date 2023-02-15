// Fill out your copyright notice in the Description page of Project Settings.


#include "GDashProjectile.h"

#include "Kismet/GameplayStatics.h"
#include "Particles/ParticleSystemComponent.h"

AGDashProjectile::AGDashProjectile()
{
	// default value
	DetonateDelay = 0.2f;
	TeleportDelay = 0.2f;

	MoveComp->InitialSpeed = 5000.0f;
}

void AGDashProjectile::BeginPlay()
{
	Super::BeginPlay();
	
	GetWorldTimerManager().SetTimer(TimerHandle_DetonateDelay,this,&AGDashProjectile::Explode,DetonateDelay);
}

void AGDashProjectile::Explode_Implementation()
{
	// 提前碰撞时正确的清空计时器
	GetWorldTimerManager().ClearTimer(TimerHandle_DetonateDelay);

	// 停下取消特效和碰撞，并生成视觉特效
	UGameplayStatics::SpawnEmitterAtLocation(this,ImpactVFX,GetActorLocation(),GetActorRotation());
	EffectComp->DeactivateSystem();
	MoveComp->StopMovementImmediately();
	SetActorEnableCollision(false);
	// 设置延迟瞬移
	FTimerHandle TimerHandle_TeleportDelay;
	GetWorldTimerManager().SetTimer(TimerHandle_TeleportDelay,this,&AGDashProjectile::Teleport,TeleportDelay);
	
}

void AGDashProjectile::Teleport()
{
	AActor* ActorToTeleport = GetInstigator();
	if(ActorToTeleport)
	{
		ActorToTeleport->TeleportTo(GetActorLocation(),ActorToTeleport->GetActorRotation());
	}
}


