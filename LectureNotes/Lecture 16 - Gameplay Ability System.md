<a name="zu6VV"></a>
## Gameplay Ability System
- Epic的Abilities, Buffs, Attributes框架 (不同于Gameplay Framework)
- 功能十分强大
- 学习曲线陡峭，文献较少
<a name="qICeP"></a>
## Build Our Own "Gameplay Ability System"

- 自己的GAS系统
   - 更好的灵活性
   - 通过打造自己的GAS系统来学习虚幻的GAS
- 代码分离度
   - 避免了一个Character类包含所有的功能和VFX
   - 单独作用的类更容易维护
   - 更好的多人合作性，尤其是在蓝图项目中
- 其他
   - 实现技能系统和BUFF系统
   - 避免通过**硬引用**在内存中加载整个游戏
   - 通过GameplayTags来管理大量的状态和标签而不是bool和enum
   - 使世界中的其他Actors也能使用Actions
<a name="bJIYx"></a>
## Tips

- UCLASS(Blueprintable) - 使该类能在虚幻蓝图中被继承，默认为NotBlueprintable，除非继承自其父类。
<a name="Gai3N"></a>
## 项目代码
Github: [https://github.com/yufeige4/ActionRoguelike](https://github.com/yufeige4/ActionRoguelike)<br />Note: 目前攻击无后摇CD，会导致连续按键鬼畜，将使用GameplayTags解决该问题

- 实现动作组件GActionComponent(ActorComponent)和动作基类GAction(UObject)
- 实现Action: Sprinting冲刺	// 冲刺Action类在蓝图中创建和实现
- 重构使用Projectile为一种基类Action
- 蓝图中创建使用火球，火焰风暴，瞬移的子类Action并设为主角ActionComp的DefaultActions
```cpp
// GAction.h
class UWorld;
/**
 * 
 */
UCLASS(Blueprintable)
class ACTIONROGUELIKE_API UGAction : public UObject
{
	GENERATED_BODY()

public:
	// nickname of an action
	UPROPERTY(EditDefaultsOnly, Category = "Action")
	FName ActionName;

	UFUNCTION(BlueprintNativeEvent, Category = "Action")
	void StartAction(AActor* Instigator);

	UFUNCTION(BlueprintNativeEvent, Category = "Action")
	void StopAction(AActor* Instigator);

	virtual UWorld* GetWorld() const override;
	
};
```
```cpp
// GAction.cpp
void UGAction::StartAction_Implementation(AActor* Instigator)
{
	UE_LOG(LogTemp,Log,TEXT("StartAction: %s"),*GetNameSafe(this));
}

void UGAction::StopAction_Implementation(AActor* Instigator)
{
	UE_LOG(LogTemp,Log,TEXT("StopAction: %s"),*GetNameSafe(this));
}

UWorld* UGAction::GetWorld() const
{
	UGActionComponent* ActionComp = Cast<UGActionComponent>(GetOuter());
	if(ActionComp)
	{
		return ActionComp->GetWorld();
	}
	return nullptr;
}
```
```cpp
// GActionComponent.h
class UGAction;

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class ACTIONROGUELIKE_API UGActionComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	
	UGActionComponent();

protected:
	
	UPROPERTY()
	TArray<UGAction*> Actions;

	UPROPERTY(EditAnywhere, Category = "Actions")
	TArray<TSubclassOf<UGAction>> DefaultActions;

public:
	
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	UFUNCTION(BlueprintCallable, Category = "Actions")
	void AddAction(TSubclassOf<UGAction> ActionClass);

	UFUNCTION(BlueprintCallable, Category = "Actions")
	bool StartActionByName(AActor* Instigator, FName ActionName);

	UFUNCTION(BlueprintCallable, Category = "Actions")
	bool StopActionByName(AActor* Instigator, FName ActionName);

protected:
	
	virtual void BeginPlay() override;
};
```
```cpp
// GActionComponent.cpp
UGActionComponent::UGActionComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
}

void UGActionComponent::BeginPlay()
{
	Super::BeginPlay();
	for(TSubclassOf<UGAction> Action : DefaultActions)
	{
		AddAction(Action);
	}
}

void UGActionComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
}

void UGActionComponent::AddAction(TSubclassOf<UGAction> ActionClass)
{
	if(!ensure(ActionClass))
	{
		return;
	}
	// UE创建新的UObject的方式
	UGAction* NewAction =  NewObject<UGAction>(this,ActionClass);
	if(ensure(NewAction))
	{
		Actions.Add(NewAction);
	}
}

bool UGActionComponent::StartActionByName(AActor* Instigator, FName ActionName)
{
	for(UGAction* Action : Actions)
	{
		if(Action && Action->ActionName==ActionName)
		{
			Action->StartAction(Instigator);
			return true;
		}
	}
	return false;
}

bool UGActionComponent::StopActionByName(AActor* Instigator, FName ActionName)
{
	for(UGAction* Action : Actions)
	{
		if(Action && Action->ActionName==ActionName)
		{
			Action->StopAction(Instigator);
			return true;
		}
	}
	return false;
}
```
```cpp
// GAction_ProjectileAttack.h
class AGCharacter;
/**
 * 
 */
UCLASS()
class ACTIONROGUELIKE_API UGAction_ProjectileAttack : public UGAction
{
	GENERATED_BODY()

protected:

	UPROPERTY(EditAnywhere,Category = "Attack")
	TSubclassOf<AActor> ProjectileClass;
	
	UPROPERTY(EditDefaultsOnly, Category = "Attack")
	float AttackAnimDelay;
	
	UPROPERTY(EditAnywhere,Category = "Attack")
	UAnimMontage* AttackAnim;

	UPROPERTY(VisibleAnywhere, Category = "Effects")
	FName HandSocketName;

	UPROPERTY(EditAnywhere, Category = "Effects")
	UParticleSystem* CastingEffect;
	
public:

	UGAction_ProjectileAttack();
	
	virtual void StartAction_Implementation(AActor* Instigator) override;
	
protected:

	UFUNCTION()
	void AttackDelay_Elapsed(AGCharacter* InstigatorCharacter,UParticleSystemComponent* CastingVFX);

};
```
```cpp
// GAction_ProjectileAttack.cpp
UGAction_ProjectileAttack::UGAction_ProjectileAttack()
{
	AttackAnimDelay = 0.5f;
	HandSocketName = "CastingSpellSocket";
}

void UGAction_ProjectileAttack::StartAction_Implementation(AActor* Instigator)
{
	Super::StartAction_Implementation(Instigator);

	AGCharacter* Character = Cast<AGCharacter>(Instigator);
	if(Character)
	{
		Character->PlayAnimMontage(AttackAnim);
		UParticleSystemComponent* CastingVFX = nullptr;
		if(CastingEffect)
		{
			CastingVFX = UGameplayStatics::SpawnEmitterAttached(CastingEffect,Character->GetMesh(),HandSocketName,FVector::ZeroVector,FRotator::ZeroRotator,EAttachLocation::SnapToTarget);
		}

		FTimerHandle TimerHandle_AttackDelay;
		FTimerDelegate TempDelegate;
		TempDelegate.BindUFunction(this,"AttackDelay_Elapsed",Character,CastingVFX);

		GetWorld()->GetTimerManager().SetTimer(TimerHandle_AttackDelay,TempDelegate,AttackAnimDelay,false);
	}
}

void UGAction_ProjectileAttack::AttackDelay_Elapsed(AGCharacter* InstigatorCharacter,UParticleSystemComponent* CastingVFX)
{
	if(IsValid(CastingVFX))
	{
		CastingVFX->DestroyComponent();
	}
	if(ensureAlways(ProjectileClass))
	{
		// 设置从手指处出现projectile
		FVector HandLocation =  InstigatorCharacter->GetMesh()->GetSocketLocation(HandSocketName);
		// 计算射线检测的参数
		FVector CameraLocation;
		FRotator CameraRotation;
		InstigatorCharacter->GetCameraViewPoint(CameraLocation,CameraRotation);
		FVector EndLocation = CameraLocation + (CameraRotation.Vector()*3000);
		
		FCollisionObjectQueryParams ObjectQueryParams;
		ObjectQueryParams.AddObjectTypesToQuery(ECC_WorldDynamic);
		ObjectQueryParams.AddObjectTypesToQuery(ECC_WorldStatic);
		ObjectQueryParams.AddObjectTypesToQuery(ECC_Pawn);
		// 射线检测忽略自身
		FCollisionQueryParams Params;
		Params.AddIgnoredActor(InstigatorCharacter);
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

		// spawn parameters
		// Projectile忽略角色本身
		FActorSpawnParameters SpawnParams;
		SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
		SpawnParams.Instigator = InstigatorCharacter;
		
		// 从世界场景中生成该Projectile
		GetWorld()->SpawnActor<AActor>(ProjectileClass,SpawnTransformMat,SpawnParams);
	}
	StopAction(InstigatorCharacter);
}
```
