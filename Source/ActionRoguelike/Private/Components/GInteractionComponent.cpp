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

	TraceDistance = 500.0f;
	TraceRadius = 30.0f;
	CollisionChannel = ECC_WorldDynamic;
	// ...
}


// Called when the game starts
void UGInteractionComponent::BeginPlay()
{
	Super::BeginPlay();
	
	OwningPawn = Cast<APawn>(GetOwner());
}


void UGInteractionComponent::FindBestInteractable()
{
	FCollisionObjectQueryParams ObjectQueryParams;
	ObjectQueryParams.AddObjectTypesToQuery(CollisionChannel);
	
	FVector End;
	
	AActor* MyOwner = GetOwner();
	FVector CameraLocation;
	FRotator CameraRotation;
	
	AGCharacter* MyCharacter = Cast<AGCharacter>(MyOwner);
	MyCharacter->GetCameraViewPoint(CameraLocation,CameraRotation);

	End = CameraLocation + (CameraRotation.Vector()*TraceDistance);
	FCollisionShape Shape;
	Shape.SetSphere(TraceRadius);
	TArray<FHitResult> Hits;
	bool bBlockingHit = GetWorld()->SweepMultiByObjectType(Hits,CameraLocation,End,FQuat::Identity,ObjectQueryParams,Shape);
	// Debug color
	FColor LineColor = bBlockingHit ? FColor::Green : FColor::Red;
	bool bDebugDraw = CVarInteractionDrawDebug.GetValueOnGameThread();

	// 清空之前保存的Focus并重新搜索
	FocusedActor = nullptr;
	
	for(FHitResult Hit : Hits)
	{
		AActor* HitActor = Hit.GetActor();
		if(HitActor!=nullptr)
		{
			// 如果该Actor实现了这个接口
			if(HitActor->Implements<UGGameplayInterface>())
			{
				FocusedActor = HitActor;
				break;
			}
		}
		// Debug Purpose
		if(bDebugDraw)
		{
			DrawDebugSphere(GetWorld(),Hit.ImpactPoint,TraceRadius,32,LineColor,false,2.0f);
		}
	}

	if(FocusedActor)
	{
		// 若当前未实例化过并且widget类被指定
		if(!HintWidgetInstance && ensure(HintWidgetClass))
		{
			HintWidgetInstance = CreateWidget<UGUserWidget_World>(GetWorld(),HintWidgetClass);
		}
		if(HintWidgetInstance)
		{
			// attach到相应的actor上, 如果未在视口中则添加到视口
			HintWidgetInstance->AttachedActor = FocusedActor;
			if(!HintWidgetInstance->IsInViewport())
			{
				HintWidgetInstance->AddToViewport();
			}
		}
	}else
	{
		// 交互物超出范围, 提示消失
		if(HintWidgetInstance && HintWidgetInstance->IsInViewport())
		{
			HintWidgetInstance->RemoveFromParent();
		}
	}
	// Debug purpose
	if(bDebugDraw)
	{
		DrawDebugLine(GetWorld(),CameraLocation,End,LineColor,false,2.0f,0,2.0f);
	}
}

// Called every fame
void UGInteractionComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
	
	if(OwningPawn->IsLocallyControlled())
	{
		FindBestInteractable();
	}
	
}

void UGInteractionComponent::PrimaryInteract()
{
	ServerInteract(FocusedActor);
}

void UGInteractionComponent::ServerInteract_Implementation(AActor* InFocusedActor)
{
	// refactored, move tracing logic into tick
	if(InFocusedActor==nullptr)
	{
		if(CVarInteractionDrawDebug.GetValueOnGameThread())
		{
			GEngine->AddOnScreenDebugMessage(-1,1.0f,FColor::Red,"No FocusedActor to Interact");
		}
		return;
	}
	APawn* MyPawn = Cast<APawn>(GetOwner());
	// 调用接口
	IGGameplayInterface::Execute_Interact(InFocusedActor,MyPawn);
	/*FCollisionObjectQueryParams ObjectQueryParams;
	ObjectQueryParams.AddObjectTypesToQuery(ECC_WorldDynamic);
	
	FVector End;
	
	AActor* MyOwner = GetOwner();
	FVector CameraLocation;
	FRotator CameraRotation;
	// 原来有缺陷 因为第三人称摄像机看向的位置和眼睛的位置不一样
	// 现已优化 使用相机的位置和方向作为发射射线检测的依据
	// MyOwner->GetActorEyesViewPoint(CameraLocation,CameraRotation);
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
	}#1#	
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
	}*/
}

