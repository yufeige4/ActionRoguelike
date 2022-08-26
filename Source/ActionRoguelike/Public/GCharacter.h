// Fill out your copyright notice in the Description page of Project Settings.
// Created by Yufei Ge 
#pragma once

#include "CoreMinimal.h"
#include "GInteractionComponent.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/Character.h"
#include "GameFramework/SpringArmComponent.h"
#include "GCharacter.generated.h"

// forward declaration
class UCameraComponent;
class USpringArmComponent;

UCLASS()
class ACTIONROGUELIKE_API AGCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	AGCharacter();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	
	UPROPERTY(VisibleAnywhere)
	USpringArmComponent* SpringArmComp;

	UPROPERTY(VisibleAnywhere)
	UCameraComponent* CameraComp;

	UPROPERTY(VisibleAnywhere)
	UGInteractionComponent* InteractionComp;
	
	void MoveForward(float val);
	void MoveRight(float val);
	
	// jump
	void jump();


public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;
	
	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	// Get position and Rotation of camera
	void GetCameraViewPoint(FVector& CameraPosition,FRotator& CameraRotation);

protected:
	
	UPROPERTY(EditAnywhere,Category = "Attack")
	TSubclassOf<AActor> ProjectileClass;

	UPROPERTY(EditAnywhere,Category = "Attack")
	UAnimMontage* AttackAnim;

	FTimerHandle TimerHandle_PrimaryAttack;

	void PrimaryAttack_TimeElapsed();
	// shoot magic projectile
	void PrimaryShoot();

	// Interact function
	void PrimaryInteract();

	
};
