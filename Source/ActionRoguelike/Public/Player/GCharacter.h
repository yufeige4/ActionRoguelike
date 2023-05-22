// Fill out your copyright notice in the Description page of Project Settings.
// Created by Yufei Ge 
#pragma once

#include "CoreMinimal.h"
#include "Components/GAttributeComponent.h"
#include "Components/GInteractionComponent.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/Character.h"
#include "GameFramework/SpringArmComponent.h"
#include "GCharacter.generated.h"

// forward declaration
class UCameraComponent;
class USpringArmComponent;
class UGActionComponent;

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
	
	UPROPERTY(VisibleAnywhere, Category = "Components")
	USpringArmComponent* SpringArmComp;

	UPROPERTY(VisibleAnywhere, Category = "Components")
	UCameraComponent* CameraComp;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	UGInteractionComponent* InteractionComp;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	UGAttributeComponent* AttributeComp;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	UGActionComponent* ActionComp;
	
public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;
	
	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	// Get position and Rotation of camera
	void GetCameraViewPoint(FVector& CameraPosition,FRotator& CameraRotation);

protected:

	UPROPERTY(VisibleDefaultsOnly, Category = "Effects")
	FName TimeToHit;

	void MoveForward(float val);
	void MoveRight(float val);

	void SprintStart();
	void SprintEnd();
	
	// jump
	void jump();
	
	// shoot fireball projectile
	void PrimaryAttack();
	// throw firestorm projectile
	void SpecialAttack();
	// dash
	void Dash();
	
	// Interact function
	void PrimaryInteract();

	UFUNCTION()
	void OnHealthChanged(AActor* InstigatorActor, UGAttributeComponent* OwningComp, float NewHealth, float Delta);
	
	void DamagedFlash();
	
	virtual void PostInitializeComponents() override;

	void Die();

public:
	UFUNCTION(Exec)
	void HealSelf(float Amount = 100);
};
