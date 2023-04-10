// Fill out your copyright notice in the Description page of Project Settings.


#include "Player/GCharacter.h"

#include "DrawDebugHelpers.h"
#include "Chaos/Private/kDOP.h"
#include "Components/CapsuleComponent.h"
#include "Components/GActionComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/KismetMathLibrary.h"
#include "Kismet/KismetSystemLibrary.h"

// Sets default values
AGCharacter::AGCharacter()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	SpringArmComp = CreateDefaultSubobject<USpringArmComponent>("SpringArmComp");
	// attach SpringArmComp to RootComp
	// RootComp is protected variable in Actor class
	SpringArmComp->SetupAttachment(RootComponent);
	SpringArmComp->bUsePawnControlRotation = true;
	CameraComp = CreateDefaultSubobject<UCameraComponent>("CameraComp");
	CameraComp->SetupAttachment(SpringArmComp);
	// 角色的Yaw不用和Controller保持同步
	bUseControllerRotationYaw = false;
	// 角色永远朝向移动的方向
	GetCharacterMovement()->bOrientRotationToMovement = true;

	// 添加Interaction组件
	InteractionComp = CreateDefaultSubobject<UGInteractionComponent>("InteractionComp");
	// 添加属性组件
	AttributeComp = CreateDefaultSubobject<UGAttributeComponent>("AttributeComp");
	// 添加Action组件
	ActionComp = CreateDefaultSubobject<UGActionComponent>("ActionComp");
	
	TimeToHit = "TimeToHit";
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
	// 使用相机的向前向量而不是角色本身的
	FRotator ControlRot = GetControlRotation();
	ControlRot.Pitch = 0.0f;
	ControlRot.Roll = 0.0f;
	AddMovementInput(ControlRot.Vector(),val);
}

// do side movement based on given input value
void AGCharacter::MoveRight(float val)
{
	// x = Forward, y = right, z is up
	// 使用相机的右向量而不是角色本身的
	FRotator ControlRot = GetControlRotation();
	ControlRot.Pitch = 0.0f;
	ControlRot.Roll = 0.0f;

	FVector RightVector = UKismetMathLibrary::GetRightVector(ControlRot);
	AddMovementInput(RightVector,val);
}

void AGCharacter::SprintStart()
{
	ActionComp->StartActionByName(this,"sprint");
}

void AGCharacter::SprintEnd()
{
	ActionComp->StopActionByName(this,"sprint");
}

void AGCharacter::PrimaryAttack()
{
	ActionComp->StartActionByName(this,"FireBallAttack");
}

void AGCharacter::SpecialAttack()
{
	ActionComp->StartActionByName(this,"FireStormAttack");
}

void AGCharacter::Dash()
{
	ActionComp->StartActionByName(this,"Dash");
}

void AGCharacter::PrimaryInteract()
{
	if(InteractionComp!=nullptr)
	{
		InteractionComp->PrimaryInteract();
	}
}

void AGCharacter::OnHealthChanged(AActor* InstigatorActor, UGAttributeComponent* OwningComp, float NewHealth, float Delta)
{
	if(Delta<0.0f)
	{
		DamagedFlash();
	}
	// 角色死亡
	if(NewHealth<=0.0f && Delta<0.0f)
	{
		Die();
	}
}

void AGCharacter::DamagedFlash()
{
	GetMesh()->SetScalarParameterValueOnMaterials(TimeToHit,GetWorld()->TimeSeconds);
}

void AGCharacter::PostInitializeComponents()
{
	Super::PostInitializeComponents();

	AttributeComp->OnHealthChanged.AddDynamic(this,&AGCharacter::OnHealthChanged);
}

void AGCharacter::Die()
{
	APlayerController* PC = Cast<APlayerController>(GetController());
	DisableInput(PC);
	auto CapsuleComp = GetCapsuleComponent();
	CapsuleComp->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	GetCharacterMovement()->DisableMovement();
}

void AGCharacter::jump()
{
	Super::Jump();
}

// Called to bind functionality to input
void AGCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	// bind certain axis input behavior to certain function call
	PlayerInputComponent->BindAxis("MoveForward",this,&AGCharacter::MoveForward);
	PlayerInputComponent->BindAxis("MoveRight",this,&AGCharacter::MoveRight);

	PlayerInputComponent->BindAxis("Turn",this,&APawn::AddControllerYawInput);
	PlayerInputComponent->BindAxis("LookUp",this,&APawn::AddControllerPitchInput);

	// sprint
	PlayerInputComponent->BindAction("Sprint",IE_Pressed,this,&AGCharacter::SprintStart);
	PlayerInputComponent->BindAction("Sprint",IE_Released,this,&AGCharacter::SprintEnd);

	// shooting magic projectile
	PlayerInputComponent->BindAction("PrimaryAttack",IE_Pressed,this,&AGCharacter::PrimaryAttack);

	// jump
	PlayerInputComponent->BindAction("Jump",IE_Pressed,this,&AGCharacter::jump);

	// 交互按键
	PlayerInputComponent->BindAction("PrimaryInteract",IE_Pressed,this,&AGCharacter::PrimaryInteract);

	// 使用特殊技能
	PlayerInputComponent->BindAction("SpecialAttack",IE_Pressed,this,&AGCharacter::SpecialAttack);

	// 使用DashProjectile
	PlayerInputComponent->BindAction("Dash",IE_Pressed,this,&AGCharacter::Dash);
}

void AGCharacter::GetCameraViewPoint(FVector& CameraPosition, FRotator& CameraRotation)
{
	CameraPosition = CameraComp->GetComponentLocation();
	CameraRotation = CameraComp->GetComponentRotation();
}

void AGCharacter::HealSelf(float Amount)
{
	AttributeComp->ApplyHealthChange(this,Amount);
}