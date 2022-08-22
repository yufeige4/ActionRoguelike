// Fill out your copyright notice in the Description page of Project Settings.


#include "GCharacter.h"

// Sets default values
AGCharacter::AGCharacter()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	SpringArmComp = CreateDefaultSubobject<USpringArmComponent>("SpringArmComp");
	// attach SpringArmComp to RootComp
	// RootComp is protected variable in Actor class
	SpringArmComp->SetupAttachment(RootComponent);
	CameraComp = CreateDefaultSubobject<UCameraComponent>("CameraComp");
	CameraComp->SetupAttachment(SpringArmComp);
	
}

// Called when the game starts or when spawned
void AGCharacter::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AGCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

// do forward movement based on given input value
void AGCharacter::MoveForward(float val)
{
	AddMovementInput(GetActorForwardVector(),val);
}

// Called to bind functionality to input
void AGCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	// bind certain axis input behavior to certain function call
	PlayerInputComponent->BindAxis("MoveForward",this,&AGCharacter::MoveForward);

	PlayerInputComponent->BindAxis("Turn",this,&APawn::AddControllerYawInput);
}

