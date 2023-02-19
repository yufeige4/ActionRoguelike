// Fill out your copyright notice in the Description page of Project Settings.


#include "RespawnedInteractableBase.h"

// Sets default values
ARespawnedInteractableBase::ARespawnedInteractableBase()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	MeshComp = CreateDefaultSubobject<UStaticMeshComponent>("MeshComp");
	RootComponent = MeshComp;

	DeltaTimeToRespawn = 0.0f;
	CanInteractAtStart = true;
	CanInteract = true;
	
}

// Called when the game starts or when spawned
void ARespawnedInteractableBase::BeginPlay()
{
	Super::BeginPlay();
	if(CanInteractAtStart)
	{
		CanInteract = true;
	}else
	{
		CanInteract = false;
		MeshComp->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		MeshComp->SetHiddenInGame(true,true);
		GetWorldTimerManager().SetTimer(TimerHandle_Respawn,this,&ARespawnedInteractableBase::Respawn,DeltaTimeToRespawn);
	}
}

// Called every frame
void ARespawnedInteractableBase::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void ARespawnedInteractableBase::Interact_Implementation(APawn* InstigatorPawn)
{
	if(CanInteract)
	{
		IGGameplayInterface::Interact_Implementation(InstigatorPawn);
		GEngine->AddOnScreenDebugMessage(0,2,FColor::Green,"Interact!");
		EndInteract_Implementation(InstigatorPawn);
	}
}

void ARespawnedInteractableBase::EndInteract_Implementation(APawn* InstigatorPawn)
{
	IGGameplayInterface::EndInteract_Implementation(InstigatorPawn);
	switch (InteractType)
	{
	case EInteractType::DoNothing:
		break;
	case EInteractType::HiddenAfterInteract:
		MeshComp->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		MeshComp->SetHiddenInGame(true,true);
		break;
	case EInteractType::DestroyAfterInteract:
		Destroy();
		return;
	}
	CanInteract = false;
	GetWorldTimerManager().SetTimer(TimerHandle_Respawn,this,&ARespawnedInteractableBase::Respawn,DeltaTimeToRespawn);
}

void ARespawnedInteractableBase::Respawn()
{
	GetWorldTimerManager().ClearTimer(TimerHandle_Respawn);
	MeshComp->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	MeshComp->SetHiddenInGame(false,true);
	CanInteract = true;
	GEngine->AddOnScreenDebugMessage(0,2,FColor::Silver,"Respawned!");
}


