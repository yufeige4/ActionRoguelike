// Fill out your copyright notice in the Description page of Project Settings.


#include "GItemChest.h"


// Sets default values
AGItemChest::AGItemChest()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	BaseMesh = CreateDefaultSubobject<UStaticMeshComponent>("BaseMesh");
	RootComponent = BaseMesh;

	LidMesh = CreateDefaultSubobject<UStaticMeshComponent>("LidMesh");
	LidMesh->SetupAttachment(BaseMesh);

	TargetRoll = -110.0f;

}

// Called when the game starts or when spawned
void AGItemChest::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AGItemChest::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AGItemChest::Interact_Implementation(APawn* InstigatorPawn)
{
	LidMesh->SetRelativeRotation(FRotator(0,0,TargetRoll));
}


