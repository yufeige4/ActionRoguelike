// Fill out your copyright notice in the Description page of Project Settings.


#include "GCharacter.h"

#include "DrawDebugHelpers.h"
#include "Chaos/Private/kDOP.h"
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

	AttackAnimDelay = 0.5f;
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

void AGCharacter::FireBallAttack_TimeElapsed()
{
	SpawnProjectile(ProjectileClass);
}

void AGCharacter::FireStormAttack_TimeElapsed()
{
	SpawnProjectile(FireStormProjectileClass);
}

void AGCharacter::Dash_TimeElapsed()
{
	SpawnProjectile(DashProjectileClass);
}

void AGCharacter::PrimaryAttack()
{
	PlayAnimMontage(AttackAnim);
	GetWorldTimerManager().SetTimer(TimerHandle_PrimaryAttack,this,&AGCharacter::FireBallAttack_TimeElapsed,AttackAnimDelay);
	// 用于当玩家死亡时 取消攻击判定
	// GetWorldTimerManager().ClearTimer(TimerHandle_PrimaryAttack);
}

void AGCharacter::SpecialAttack()
{
	PlayAnimMontage(AttackAnim);
	GetWorldTimerManager().SetTimer(TimerHandle_SpecialAttack,this,&AGCharacter::FireStormAttack_TimeElapsed,AttackAnimDelay);
}

void AGCharacter::Dash()
{
	PlayAnimMontage(AttackAnim);
	GetWorldTimerManager().SetTimer(TimerHandle_Dash,this,&AGCharacter::Dash_TimeElapsed,AttackAnimDelay);
}

void AGCharacter::PrimaryInteract()
{
	if(InteractionComp!=nullptr)
	{
		InteractionComp->PrimaryInteract();
	}
}

void AGCharacter::SpawnProjectile(TSubclassOf<AActor> ClassToSpawn)
{
	// make sure the projectile is set up
	if(ensureAlways(ClassToSpawn))
	{
		// 设置从手指处出现projectile
		FVector HandLocation =  GetMesh()->GetSocketLocation("RightHandMiddle4");
		// 计算射线检测的参数
		FVector CameraLocation;
		FRotator CameraRotation;
		this->GetCameraViewPoint(CameraLocation,CameraRotation);
		FVector EndLocation = CameraLocation + (CameraRotation.Vector()*3000);
		
		FCollisionObjectQueryParams ObjectQueryParams;
		ObjectQueryParams.AddObjectTypesToQuery(ECC_WorldDynamic);
		ObjectQueryParams.AddObjectTypesToQuery(ECC_WorldStatic);
		ObjectQueryParams.AddObjectTypesToQuery(ECC_Pawn);
		// 射线检测忽略自身
		FCollisionQueryParams Params;
		Params.AddIgnoredActor(this);
		// 设置碰撞球体检测参数
		FCollisionShape Shape;
		Shape.SetSphere(20.0f);

		
		FHitResult Hit;
		bool bBlockingHit = GetWorld()->SweepSingleByObjectType(Hit,CameraLocation,EndLocation,FQuat::Identity,ObjectQueryParams,Shape,Params);
		FVector ImpactLocation = EndLocation;
		if(bBlockingHit)
		{
			// 命中
			ImpactLocation = Hit.ImpactPoint;
		}
		// 使用命中位置和手的位置来计算火球方向
		FRotator SpawnRotation = FRotationMatrix::MakeFromX(ImpactLocation-HandLocation).Rotator();

		// FColor LineColor = bBlockingHit ? FColor::Green : FColor::Red;
		// DrawDebugLine(GetWorld(),CameraLocation,EndLocation,LineColor,false,2.0f,0,2.0f);
	
		// set up spawn transform for projectile
		FTransform SpawnTransformMat = FTransform(SpawnRotation,HandLocation);

		// Projectile忽略角色本身
		FActorSpawnParameters SpawnParams;
		SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
		SpawnParams.Instigator = this;
		
		// 从世界场景中生成该Projectile
		GetWorld()->SpawnActor<AActor>(ClassToSpawn,SpawnTransformMat,SpawnParams);
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
	PlayerInputComponent->BindAction("PrimaryAttack",IE_Pressed,this,&AGCharacter::PrimaryAttack);

	// jump
	PlayerInputComponent->BindAction("Jump",IE_Pressed,this,&AGCharacter::jump);

	// 交互按键
	PlayerInputComponent->BindAction("PrimaryInteract",IE_Pressed,this,&AGCharacter::PrimaryInteract);

	// 使用特殊技能
	PlayerInputComponent->BindAction("SpecialAttack",IE_Pressed,this,&AGCharacter::SpecialAttack);

	// 使用Dash
	PlayerInputComponent->BindAction("Dash",IE_Pressed,this,&AGCharacter::Dash);
}

void AGCharacter::GetCameraViewPoint(FVector& CameraPosition, FRotator& CameraRotation)
{
	CameraPosition = CameraComp->GetComponentLocation();
	CameraRotation = CameraComp->GetComponentRotation();
}

