// Fill out your copyright notice in the Description page of Project Settings.


#include "GCharacter.h"
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

void AGCharacter::PrimaryAttack_TimeElapsed()
{
	// 设置从手指处出现火球
	FVector HandLocation =  GetMesh()->GetSocketLocation("RightHandMiddle4");
	// set up spawn transform for projectile
	FTransform SpawnTransformMat = FTransform(GetControlRotation(),HandLocation);
	
	FActorSpawnParameters SpawnParams;
	SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

	// 从世界场景中生成该Projectile
	GetWorld()->SpawnActor<AActor>(ProjectileClass,SpawnTransformMat,SpawnParams);
}

void AGCharacter::PrimaryShoot()
{
	PlayAnimMontage(AttackAnim);
	GetWorldTimerManager().SetTimer(TimerHandle_PrimaryAttack,this,&AGCharacter::PrimaryAttack_TimeElapsed,0.5f);
	// 用于当玩家死亡时 取消攻击判定
	// GetWorldTimerManager().ClearTimer(TimerHandle_PrimaryAttack);
}

void AGCharacter::PrimaryInteract()
{
	if(InteractionComp!=nullptr)
	{
		InteractionComp->PrimaryInteract();
	}
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

	// shooting magic projectile
	PlayerInputComponent->BindAction("PrimaryShoot",IE_Pressed,this,&AGCharacter::PrimaryShoot);

	// jump
	PlayerInputComponent->BindAction("Jump",IE_Pressed,this,&AGCharacter::jump);

	// 交互按键
	PlayerInputComponent->BindAction("PrimaryInteract",IE_Pressed,this,&AGCharacter::PrimaryInteract);
}

void AGCharacter::GetCameraViewPoint(FVector& CameraPosition, FRotator& CameraRotation)
{
	CameraPosition = CameraComp->GetComponentLocation();
	CameraRotation = CameraComp->GetComponentRotation();
}

