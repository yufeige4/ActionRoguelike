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

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	UGAttributeComponent* AttributeComp;
	
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
	TSubclassOf<AActor> FireStormProjectileClass;

	UPROPERTY(EditAnywhere,Category = "Attack")
	TSubclassOf<AActor> DashProjectileClass;

	UPROPERTY(EditAnywhere,Category = "Attack")
	UAnimMontage* AttackAnim;

	float AttackAnimDelay;

	FTimerHandle TimerHandle_PrimaryAttack;
	FTimerHandle TimerHandle_SpecialAttack;
	FTimerHandle TimerHandle_Dash;
	
	void FireBallAttack_TimeElapsed();
	void FireStormAttack_TimeElapsed();
	void Dash_TimeElapsed();
	
	// shoot fireball projectile
	void PrimaryAttack();
	// throw firestorm projectile
	void SpecialAttack();
	// dash
	void Dash();
	
	// Interact function
	void PrimaryInteract();

	// extract common part of spawn projectile
	void SpawnProjectile(TSubclassOf<AActor> ClassToSpawn);

	UFUNCTION()
	void OnHealthChanged(AActor* InstigatorActor, UGAttributeComponent* OwningComp, float NewHealth, float Delta);
	
	void DamagedFlash();
	
	virtual void PostInitializeComponents() override;
};
