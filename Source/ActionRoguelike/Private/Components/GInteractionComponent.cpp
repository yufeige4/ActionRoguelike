// Fill out your copyright notice in the Description page of Project Settings.


#include "Components/GInteractionComponent.h"

#include "DrawDebugHelpers.h"
#include "Player/GCharacter.h"
#include "Core/GGameplayInterface.h"

static TAutoConsoleVariable<bool> CVarInteractionDrawDebug(TEXT("ARPG.InteractDrawDebug"),false,TEXT("toggle whether draw debug info for InteractionComp"),ECVF_Cheat);
// Sets default values for this component's properties
UGInteractionComponent::UGInteractionComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...
}


// Called when the game starts
void UGInteractionComponent::BeginPlay()
{
	Super::BeginPlay();

	// ...
	
}


// Called every frame
void UGInteractionComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
}

void UGInteractionComponent::PrimaryInteract()
{
	FCollisionObjectQueryParams ObjectQueryParams;
	ObjectQueryParams.AddObjectTypesToQuery(ECC_WorldDynamic);
	
	FVector End;
	
	AActor* MyOwner = GetOwner();
	FVector CameraLocation;
	FRotator CameraRotation;
	// 原来有缺陷 因为第三人称摄像机看向的位置和眼睛的位置不一样
	// 现已优化 使用相机的位置和方向作为发射射线检测的依据
	AGCharacter* MyCharacter = Cast<AGCharacter>(MyOwner);
	MyCharacter->GetCameraViewPoint(CameraLocation,CameraRotation);

	End = CameraLocation + (CameraRotation.Vector()*500);
	// 线性射线检测
	// 缺陷1 以后可以使用Channel来优化
	// 缺陷2 过于精准
	// FHitResult Hit;
	// bool bBlockingHit = GetWorld()->LineTraceSingleByObjectType(Hit,CameraLocation,End,ObjectQueryParams);
	/*AActor* HitActor = Hit.GetActor();
	if(HitActor!=nullptr)
	{
		// 如果该Actor实现了这个接口
		if(HitActor->Implements<UGGameplayInterface>())
		{
			APawn* MyPawn = Cast<APawn>(MyOwner);
			// 调用接口
			IGGameplayInterface::Execute_Interact(HitActor,MyPawn);
		}
	}*/	
	FCollisionShape Shape;
	float Radius = 30.0f;
	Shape.SetSphere(Radius);
	TArray<FHitResult> Hits;
	bool bBlockingHit = GetWorld()->SweepMultiByObjectType(Hits,CameraLocation,End,FQuat::Identity,ObjectQueryParams,Shape);
	// Debug color
	FColor LineColor = bBlockingHit ? FColor::Green : FColor::Red;
	bool bDebugDraw = CVarInteractionDrawDebug.GetValueOnGameThread();
	
	for(FHitResult Hit : Hits)
	{
		AActor* HitActor = Hit.GetActor();
		if(HitActor!=nullptr)
		{
			// 如果该Actor实现了这个接口
			if(HitActor->Implements<UGGameplayInterface>())
			{
				APawn* MyPawn = Cast<APawn>(MyOwner);
				// 调用接口
				IGGameplayInterface::Execute_Interact(HitActor,MyPawn);
				// 只和第一个物体交互
				break;
			}
		}
		// Debug Purpose
		if(bDebugDraw)
		{
			DrawDebugSphere(GetWorld(),Hit.ImpactPoint,Radius,32,LineColor,false,2.0f);
		}
	}
	
	// Debug purpose
	if(bDebugDraw)
	{
		DrawDebugLine(GetWorld(),CameraLocation,End,LineColor,false,2.0f,0,2.0f);
	}
}

