<a name="brWVq"></a>
## Brain Component
BehaviorTreeComponent的父类，可以通过调用StopLogic()函数停止行为树
<a name="ZU8ff"></a>
## Static Function
静态函数
<a name="ClukE"></a>
## Metadata Specifier
在声明类、接口、结构、枚举、枚举值、函数或属性时，你可以添加**元数据指示器**来控制它们如何与引擎和编辑器的各个方面互动<br />使用方式：<br />{UCLASS/UENUM/UINTERFACE/USTRUCT/UFUNCTION/UPROPERTY}(SpecifierX, meta=(MetaTag1="Value1", MetaTag2, ..), SpecifierY)
<a name="nCvQi"></a>
## Tips

- SceneComponent需要在创建时设置Attachment，因为他们有位置信息
<a name="GAUgm"></a>
## 项目代码

- 实现怪物死亡逻辑
- 实现受击索敌
- 实现两个静态函数，获取属性组件和判断是否存活
- 赋予怪物攻击随机性
- 玩家死亡后怪物停止攻击
- 怪物受击闪烁
```cpp
// GAICharacter.cpp
// ...
void AGAICharacter::Die()
{
	// stop BT
	AAIController* AIC = Cast<AAIController>(GetController());
	if(AIC)
	{
		AIC->GetBrainComponent()->StopLogic("Killed");
	}
	// Ragdoll
	GetMesh()->SetAllBodiesSimulatePhysics(true);
	GetMesh()->SetCollisionProfileName("Ragdoll"); 
	Cast<UCapsuleComponent>(RootComponent)->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		
	// SetLifeSpan
	SetLifeSpan(10.0f);
}

void AGAICharacter::OnChangedHealth(AActor* InstigatorActor, UGAttributeComponent* OwningComp, float NewHealth,
	float Delta)
{
	if(Delta<0.0f)
	{
		DamagedFlash();
		// can be improve by filter out ally
		if(InstigatorActor!=this)
		{
			SetTargetActor(InstigatorActor);
		}
		if(NewHealth<=0)
		{
			Die();
		}
	}
}

void AGAICharacter::Attack_Implementation(AActor* TargetActor)
{
	IGAICharacterInterface::Attack_Implementation(TargetActor);

	FVector SpawnLocation = GetMesh()->GetSocketLocation("Muzzle_Front_XForward");
	FVector TargetLocation = TargetActor->GetActorLocation();
	FRotator SpawnRotation = FRotationMatrix::MakeFromX(TargetLocation-SpawnLocation).Rotator();
	// Give random offset to projectile to make game more fun
	SpawnRotation.Pitch += FMath::RandRange(0.0f,RandomAttackOffset);
	SpawnRotation.Yaw += FMath::RandRange(-RandomAttackOffset,RandomAttackOffset);
	
	auto RangeAttack_TimeElapsed = [=]()
	{
		SpawnProjectile(RangeAttackProjectile,SpawnLocation,SpawnRotation);
	};
	PlayAnimMontage(RangeAttackAnim);
	GetWorldTimerManager().SetTimer(TimerHandle_RangeAttack,RangeAttack_TimeElapsed,1,false,RangeAttackProjectileDelay);
}
```
```cpp
// GAttributeComponent.h
// ...
public:
	UFUNCTION(BlueprintCallable, Category = "Attributes")
	bool ApplyHealthChange(AActor* Instigator, float Delta);

	UFUNCTION(BlueprintCallable, Category = "Attributes")
	static UGAttributeComponent* GetAttributeComponent(AActor* Actor);
	
	UFUNCTION(BlueprintCallable, Category = "Attributes", meta = (DisplayName = "IsAlive"))
	static bool IsActorAlive(AActor* Actor);
```
```cpp
// GAttributeComponent.cpp
// ... 
bool UGAttributeComponent::ApplyHealthChange(AActor* Instigator, float Delta)
{
	float PreviousHealth = CurrHealth;
	CurrHealth = FMath::Clamp(CurrHealth+Delta,0.0f,MaxHealth);
	float AcutualDelta = CurrHealth - PreviousHealth;
	// 调用代理
	OnHealthChanged.Broadcast(Instigator,this,CurrHealth,AcutualDelta);
	
	return true;
}

UGAttributeComponent* UGAttributeComponent::GetAttributeComponent(AActor* Actor)
{
	if(Actor)
	{
		return Cast<UGAttributeComponent>(Actor->GetComponentByClass(UGAttributeComponent::StaticClass()));
	}
	return nullptr;
}

// return false if no AttributeComp or not alive
bool UGAttributeComponent::IsActorAlive(AActor* Actor)
{
	UGAttributeComponent* AttributeComp = GetAttributeComponent(Actor);
	return (AttributeComp && AttributeComp->IsAlive());
}
```
```cpp
// BTTask_AttackTarget.cpp
// ...
EBTNodeResult::Type UBTTask_AttackTarget::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	AAIController* MyController = OwnerComp.GetAIOwner();
	UBlackboardComponent* MyBB = OwnerComp.GetBlackboardComponent();

	if(ensure(MyController)&ensure(MyBB))
	{
		APawn* MyPawn = MyController->GetPawn();
		if(MyPawn && MyPawn->Implements<UGAICharacterInterface>())
		{
			AActor* TargetActor = Cast<AActor>(MyBB->GetValueAsObject(Key_TargetActor.SelectedKeyName));
			if(TargetActor && UGAttributeComponent::IsActorAlive(TargetActor))
			{
				IGAICharacterInterface::Execute_Attack(MyPawn,TargetActor);
				return EBTNodeResult::Succeeded;
			}
		}
		
	}
	return EBTNodeResult::Failed;
}
```
