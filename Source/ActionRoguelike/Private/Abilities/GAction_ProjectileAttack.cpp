// Fill out your copyright notice in the Description page of Project Settings.


#include "Abilities/GAction_ProjectileAttack.h"
#include "Kismet/GameplayStatics.h"
#include "Player/GCharacter.h"

UGAction_ProjectileAttack::UGAction_ProjectileAttack()
{
	AttackAnimDelay = 0.5f;
	HandSocketName = "CastingSpellSocket";
}

void UGAction_ProjectileAttack::StartAction_Implementation(AActor* Instigator)
{
	Super::StartAction_Implementation(Instigator);

	AGCharacter* Character = Cast<AGCharacter>(Instigator);
	if(Character)
	{
		Character->PlayAnimMontage(AttackAnim);
		UParticleSystemComponent* CastingVFX = nullptr;
		if(CastingEffect)
		{
			CastingVFX = UGameplayStatics::SpawnEmitterAttached(CastingEffect,Character->GetMesh(),HandSocketName,FVector::ZeroVector,FRotator::ZeroRotator,EAttachLocation::SnapToTarget);
		}

		FTimerHandle TimerHandle_AttackDelay;
		FTimerDelegate TempDelegate;
		TempDelegate.BindUFunction(this,"AttackDelay_Elapsed",Character,CastingVFX);

		GetWorld()->GetTimerManager().SetTimer(TimerHandle_AttackDelay,TempDelegate,AttackAnimDelay,false);
	}
}

void UGAction_ProjectileAttack::AttackDelay_Elapsed(AGCharacter* InstigatorCharacter,UParticleSystemComponent* CastingVFX)
{
	if(IsValid(CastingVFX))
	{
		CastingVFX->DestroyComponent();
	}
	if(ensureAlways(ProjectileClass))
	{
		// 设置从手指处出现projectile
		FVector HandLocation =  InstigatorCharacter->GetMesh()->GetSocketLocation(HandSocketName);
		// 计算射线检测的参数
		FVector CameraLocation;
		FRotator CameraRotation;
		InstigatorCharacter->GetCameraViewPoint(CameraLocation,CameraRotation);
		FVector EndLocation = CameraLocation + (CameraRotation.Vector()*3000);
		
		FCollisionObjectQueryParams ObjectQueryParams;
		ObjectQueryParams.AddObjectTypesToQuery(ECC_WorldDynamic);
		ObjectQueryParams.AddObjectTypesToQuery(ECC_WorldStatic);
		ObjectQueryParams.AddObjectTypesToQuery(ECC_Pawn);
		// 射线检测忽略自身
		FCollisionQueryParams Params;
		Params.AddIgnoredActor(InstigatorCharacter);
		// 设置碰撞球体检测参数
		FCollisionShape Shape;
		Shape.SetSphere(20.0f);

		
		FHitResult Hit;
		bool bBlockingHit = GetWorld()->SweepSingleByObjectType(Hit,CameraLocation,EndLocation,FQuat::Identity,ObjectQueryParams,Shape,Params);
		FVector ImpactLocation = EndLocation;
		if(bBlockingHit)
		{
			// 命中
			ImpactLocation = Hit.ImpactPoint;
		}
		// 使用命中位置和手的位置来计算火球方向
		FRotator SpawnRotation = FRotationMatrix::MakeFromX(ImpactLocation-HandLocation).Rotator();

		// FColor LineColor = bBlockingHit ? FColor::Green : FColor::Red;
		// DrawDebugLine(GetWorld(),CameraLocation,EndLocation,LineColor,false,2.0f,0,2.0f);
	
		// set up spawn transform for projectile
		FTransform SpawnTransformMat = FTransform(SpawnRotation,HandLocation);

		// spawn parameters
		// Projectile忽略角色本身
		FActorSpawnParameters SpawnParams;
		SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
		SpawnParams.Instigator = InstigatorCharacter;
		
		// 从世界场景中生成该Projectile
		GetWorld()->SpawnActor<AActor>(ProjectileClass,SpawnTransformMat,SpawnParams);
	}
	StopAction(InstigatorCharacter);
}
