// Fill out your copyright notice in the Description page of Project Settings.


#include "Projectiles/GMagicProjectile.h"
#include "GGameplayFunctionLibrary.h"
#include "AudioMixerXAudio2/Private/AudioMixerPlatformXAudio2.h"
#include "Components/GActionComponent.h"
#include "Components/SphereComponent.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Particles/ParticleSystemComponent.h"
#include "Abilities/GActionEffect.h"

// Sets default values
AGMagicProjectile::AGMagicProjectile()
{
	SphereComp->SetSphereRadius(20.0f);
	SphereComp->OnComponentBeginOverlap.AddDynamic(this,&AGMagicProjectile::OnActorOverlap);
	
	DamageAmount = 20.0f;
	TimeToSelfDestroy = 2.0f;
}

void AGMagicProjectile::BeginPlay()
{
	Super::BeginPlay();
	GetWorldTimerManager().SetTimer(TimerHandle_selfDestroy,this,&AGMagicProjectile::SelfDestroy,TimeToSelfDestroy);
}


void AGMagicProjectile::SelfDestroy()
{
	GetWorldTimerManager().ClearTimer(TimerHandle_selfDestroy);
	/*if(GEngine)
	{
		GEngine->AddOnScreenDebugMessage(0, 2, FColor::Black, TEXT("SelfDestroyed!"));
	}*/
	Destroy();
}

void AGMagicProjectile::Explode_Implementation()
{
	GetWorldTimerManager().ClearTimer(TimerHandle_selfDestroy);
	UGameplayStatics::PlaySoundAtLocation(GetWorld(),ImpactSound,GetActorLocation(),GetActorRotation());
	// 爆炸相机抖动
	UGameplayStatics::PlayWorldCameraShake(GetWorld(),ImpactShake,GetActorLocation(),0.0f,2500.0f);
	Super::Explode_Implementation();
}

void AGMagicProjectile::OnActorOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
                                       UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if(OtherActor && OtherActor!=GetInstigator())
	{
		UGActionComponent* ActionComp = UGActionComponent::GetActionComponent(OtherActor);
		if(ActionComp && ActionComp->ActiveGameplayTags.HasTag(ParryTag))
		{
			// 识别到反弹BUFF
			MoveComp->Velocity = -MoveComp->Velocity;
			// 改变魔法导弹的所有权
			SetInstigator(Cast<APawn>(OtherActor));
			return;
		}
		
		if(IgnoreActors.Find(OtherActor)!=INDEX_NONE)
		{
			return;
		}
		IgnoreActors.Push(OtherActor);

		if(UGGameplayFunctionLibrary::ApplyDirectionalDamage(GetInstigator(),OtherActor,-DamageAmount,SweepResult))
		{
			if(ActionComp && AppliedEffect)
			{
				// 添加Debuff
				ActionComp->AddAction(AppliedEffect,GetInstigator());
			}
			Explode();
		}
	}
}

