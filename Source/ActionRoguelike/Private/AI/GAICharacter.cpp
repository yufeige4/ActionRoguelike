// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/GAICharacter.h"

// Sets default values
AGAICharacter::AGAICharacter()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void AGAICharacter::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AGAICharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

float AGAICharacter::GetAttackRange_Implementation()
{
	return AttackRange;
}


