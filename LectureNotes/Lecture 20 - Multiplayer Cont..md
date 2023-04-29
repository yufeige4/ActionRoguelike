<a name="RzCT8"></a>
## Gameplay Classes
![image.png](https://cdn.nlark.com/yuque/0/2023/png/32753014/1682671477560-5bf56b6c-b92a-40b0-87bf-8a618f62a357.png#averageHue=%232f2d27&clientId=uc2e39a6d-1575-4&from=paste&height=433&id=u176891bb&originHeight=595&originWidth=1039&originalType=binary&ratio=1.375&rotation=0&showTitle=false&size=225175&status=done&style=none&taskId=ud2fbbb01-8680-4336-a109-9f8b97c3c7e&title=&width=755.6363636363636)<br />![image.png](https://cdn.nlark.com/yuque/0/2023/png/32753014/1682683752796-2ef412ef-4189-4cd5-90d8-71ca46062ed3.png#averageHue=%23493a28&clientId=uc2e39a6d-1575-4&from=paste&height=281&id=ufa1b6102&originHeight=386&originWidth=1146&originalType=binary&ratio=1.375&rotation=0&showTitle=false&size=95100&status=done&style=none&taskId=u1988da64-e392-4735-bae6-67f9da21c4f&title=&width=833.4545454545455)<br />![image.png](https://cdn.nlark.com/yuque/0/2023/png/32753014/1682683839039-a532d6d4-fa0a-4170-9ac5-f32626fac7bd.png#averageHue=%23eca139&clientId=uc2e39a6d-1575-4&from=paste&height=405&id=u5edaf2d3&originHeight=557&originWidth=1042&originalType=binary&ratio=1.375&rotation=0&showTitle=false&size=107608&status=done&style=none&taskId=ud3b59736-a149-4c42-b51e-7e3b7814bd4&title=&width=757.8181818181819)
<a name="CGEQo"></a>
## Tips

- PlayerController只存在于**本地客户端**和**服务器**上，不在**其他客户端**上存在
- GameMode只存在于**服务器**上
- 所有的Pawn会在全部**客户端**和**服务器**上拷贝一份
- 需要将数据转换为以**服务器**为权威的
- RepNotify适用于状态发生改变
   - 当新玩家加入游戏，会将已改变的状态全部同步正常
   - 对应用范围外事件优化时也有帮助
- NetMulticast适用于作为一个过渡函数 - Momentary Event
   - 当新玩家加入游戏，则不会同步已改变的状态
- Meta:
   - ExposeOnSpawn - 在蓝图创建时的节点上可以修改
- 当在蓝图中使用RPC时需要注意后续其他功能设置延迟 // 例如DestroyActor，可使用SetLifeSpan替代
   - ![image.png](https://cdn.nlark.com/yuque/0/2023/png/32753014/1682761792413-299988cd-8f6d-44d0-b9aa-f5150cc88330.png#averageHue=%232a2826&clientId=uefbc8ade-04e7-4&from=paste&height=706&id=ucb83cc2c&originHeight=971&originWidth=1518&originalType=binary&ratio=1.375&rotation=0&showTitle=false&size=240944&status=done&style=none&taskId=u82da7fee-4f48-41a8-9fd5-0a23ab48199&title=&width=1104)
<a name="v8ghC"></a>
## 项目代码
GitHub: [https://github.com/yufeige4/ActionRoguelike](https://github.com/yufeige4/ActionRoguelike)

- 修复PlayerController未判断是否为本地导致的报错
- 将刷新道具变为Replicated的
- 使用NetMulticast方法将AttributeComponent支持多人游戏
- 怪物AI简单修复 // 将projectile变成replicated
- 对ActionComponent(ActorComp)的StartAction函数进行了简单的处理，支持多人游戏
```cpp
// GAttributeComponent.h
// 代理声明
DECLARE_DYNAMIC_MULTICAST_DELEGATE_FourParams(FOnHealthChanged, AActor*, InstigatorActor, UGAttributeComponent*, OwningComp, float, NewHealth, float, Delta);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_FiveParams(FOnRageChanged, AActor*, InstigatorActor, UGAttributeComponent*, OwningComp, int32, NewRage, int32, Delta, bool, IsSuccess);

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class ACTIONROGUELIKE_API UGAttributeComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UGAttributeComponent();

protected:
	// 当前生命值
	UPROPERTY(Replicated, EditDefaultsOnly, BlueprintReadOnly, Category = "Attributes")
	float CurrHealth;
	// 最大生命值
	UPROPERTY(Replicated, EditDefaultsOnly, BlueprintReadOnly, Category = "Attributes")
	float MaxHealth;
	// 当前怒气
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Attributes")
	int32 CurrRage;
	// 愤怒值变化量
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Attributes")
	int32 RageGrowthRate;
	// 最大怒气
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Attributes")
	int32 MaxRage;
	
public:
	// 判断怒气值是否足够
	UFUNCTION(BlueprintCallable, Category = "Attributes")
	bool HaveEnoughRage(int32 RageCost);
	// 对怒气值进行修改
	UFUNCTION(BlueprintCallable, Category = "Attributes")
	bool ApplyRageChange(AActor* Instigator, int32 Delta);
	// 包含代理
	UPROPERTY(BlueprintAssignable, Category = "Attributes")
	FOnRageChanged OnRageChanged;
	// 对生命值进行修改
	UFUNCTION(BlueprintCallable, Category = "Attributes")
	bool ApplyHealthChange(AActor* Instigator, float Delta);
	// 包含代理
	UPROPERTY(BlueprintAssignable, Category = "Attributes")
	FOnHealthChanged OnHealthChanged;

	UFUNCTION(BlueprintCallable, Category = "Attributes")
	bool IsAlive() const;

	UFUNCTION(BlueprintCallable, Category = "Attributes")
	bool IsFullHealth() const;

	UFUNCTION(BlueprintCallable, Category = "Attributes")
	static UGAttributeComponent* GetAttributeComponent(AActor* Actor);
	
	UFUNCTION(BlueprintCallable, Category = "Attributes", meta = (DisplayName = "IsAlive"))
	static bool IsActorAlive(AActor* Actor);

	UFUNCTION(BlueprintCallable, Category = "Attributes")
	float GetCurrHealth();

	UFUNCTION(BlueprintCallable, Category = "Attributes")
	float GetMaxHealth();

	UFUNCTION(BlueprintCallable, Category = "Attributes")
	float GetMaxRage();
	
	UFUNCTION(BlueprintCallable, Category = "Attributes")
	bool Kill(AActor* Instigator);

protected:

	UFUNCTION(NetMulticast, Reliable) // @FIXME: mark as unreliable once moved the "state" of PlayerCharacter
	void MulticastHealthChanged(AActor* Instigator, float NewHealth, float Delta);
	
};
```
```cpp
// GAttributeComponent.cpp
static TAutoConsoleVariable<float> CVarDamageMultiplier(TEXT("ARPG.DamageMultiplier"),1.0f,TEXT("Global ratio of damage dealing"),ECVF_Cheat);
// Sets default values for this component's properties
UGAttributeComponent::UGAttributeComponent()
{
	CurrHealth = 100.0f;
	MaxHealth = 100.0f;
	CurrRage = 0;
	RageGrowthRate = 5;
	MaxRage = 100;

	SetIsReplicatedByDefault(true);
}

bool UGAttributeComponent::HaveEnoughRage(int32 RageCost)
{
	return (CurrRage-RageCost)>=0;
}

bool UGAttributeComponent::ApplyRageChange(AActor* Instigator, int32 Delta)
{
	if(Delta==0)
	{
		return false;
	}
	int32 NewRage = FMath::Clamp(CurrRage+Delta,0,MaxRage);
	int32 ActualDelta = NewRage-CurrRage;
	bool bSuccess = true;
	if(Delta<0)
	{
		bSuccess = Delta==ActualDelta ? true : false;
	}
	if(bSuccess)
	{
		CurrRage = NewRage;
	}
	OnRageChanged.Broadcast(Instigator,this,CurrRage,ActualDelta,bSuccess);
	return bSuccess;
}

bool UGAttributeComponent::ApplyHealthChange(AActor* Instigator, float Delta)
{
	if(!GetOwner()->CanBeDamaged() && Delta<0.0f)
	{
		return false;		
	}
	
	float Ratio = 1;
	if(Delta<0.0f)
	{
		Ratio = CVarDamageMultiplier.GetValueOnGameThread();
		Delta *= Ratio;
	}
	
	float PreviousHealth = CurrHealth;
	CurrHealth = FMath::Clamp(CurrHealth+Delta,0.0f,MaxHealth);
	float AcutualDelta = CurrHealth - PreviousHealth;

	if(AcutualDelta<0.0f)
	{
		ApplyRageChange(Instigator,RageGrowthRate);
	}
	
	// 调用代理
	// OnHealthChanged.Broadcast(Instigator,this,CurrHealth,AcutualDelta);
	if(AcutualDelta!=0.0f)
	{
		MulticastHealthChanged(Instigator,CurrHealth,AcutualDelta);
	}
	// Died
	if(AcutualDelta<0.0f && CurrHealth==0.0f)
	{
		auto Owner = GetOwner();
		auto EventManager = UGEventManager::GetEventManager(Owner);
		if(EventManager)
		{
			EventManager->OnActorKilled(Owner,Instigator);
		}
	}
	return true;
}

bool UGAttributeComponent::IsAlive() const
{
	return CurrHealth > 0.0f;
}

bool UGAttributeComponent::IsFullHealth() const
{
	return CurrHealth==MaxHealth;
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

float UGAttributeComponent::GetCurrHealth()
{
	return CurrHealth;
}

float UGAttributeComponent::GetMaxHealth()
{
	return MaxHealth;
}

float UGAttributeComponent::GetMaxRage()
{
	return MaxRage;
}

bool UGAttributeComponent::Kill(AActor* Instigator)
{
	return ApplyHealthChange(Instigator,-MaxHealth);
}

void UGAttributeComponent::MulticastHealthChanged_Implementation(AActor* Instigator, float NewHealth, float Delta)
{
	OnHealthChanged.Broadcast(Instigator,this,NewHealth,Delta);
}

void UGAttributeComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(UGAttributeComponent,CurrHealth);
	DOREPLIFETIME(UGAttributeComponent,MaxHealth);
	// if MaxHealth is changed, only the owner can see it, for saving bandwidth
	// DOREPLIFETIME_CONDITION(UGAttributeComponent,CurrHealth,COND_OwnerOnly);
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

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Tags")
	FGameplayTagContainer ActiveGameplayTags;

protected:
	
	UPROPERTY()
	TArray<UGAction*> Actions;

	UPROPERTY(EditAnywhere, Category = "Actions")
	TArray<TSubclassOf<UGAction>> DefaultActions;

public:
	
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	UFUNCTION(BlueprintCallable, Category = "Actions")
	UGAction* AddAction(TSubclassOf<UGAction> ActionClass, AActor* Instigator);

	UFUNCTION(BlueprintCallable, Category = "Actions")
	void RemoveAction(UGAction* Action);

	UFUNCTION(BlueprintCallable, Category = "Actions")
	bool StartActionByName(AActor* Instigator, FName ActionName);

	UFUNCTION(BlueprintCallable, Category = "Actions")
	bool StopActionByName(AActor* Instigator, FName ActionName);
	
	UFUNCTION(BlueprintCallable, Category = "Actions")
	static UGActionComponent* GetActionComponent(AActor* Actor);

protected:
	
	virtual void BeginPlay() override;

	UFUNCTION(Server, Reliable)
	void ServerStartAction(AActor* Instigator, FName ActionName);

	UFUNCTION(Server, Reliable)
	void ServerStopAction(AActor* Instigator, FName ActionName);

};
```
```cpp
// GActionComponent.cpp
UGActionComponent::UGActionComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
	SetIsReplicatedByDefault(true);
}

void UGActionComponent::BeginPlay()
{
	Super::BeginPlay();
	for(TSubclassOf<UGAction> Action : DefaultActions)
	{
		// add from default actions should consider owner of comp as Instigator
		AddAction(Action,GetOwner());
	}
}

void UGActionComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	if(DebugActionTag.GetValueOnGameThread())
	{
		if(GEngine)
		{
			FString DebugMsg = GetNameSafe(GetOwner()) + " : " + ActiveGameplayTags.ToStringSimple();
			GEngine->AddOnScreenDebugMessage(-1,0.0,FColor::White,DebugMsg);
		}
	}
}

UGAction* UGActionComponent::AddAction(TSubclassOf<UGAction> ActionClass, AActor* Instigator)
{
	if(!ensure(ActionClass))
	{
		return nullptr;
	}
	// UE创建新的UObject的方式
	UGAction* NewAction =  NewObject<UGAction>(this,ActionClass);
	if(ensure(NewAction))
	{
		Actions.Add(NewAction);
		if(NewAction->bAutoStart && ensure(NewAction->CanStart(Instigator)))
		{
			NewAction->StartAction(Instigator);
		}
	}
	return NewAction;
}

void UGActionComponent::RemoveAction(UGAction* Action)
{
	if(!ensure(Action && !Action->IsRunning()))
	{
		return;
	}
	Actions.Remove(Action);
}

bool UGActionComponent::StartActionByName(AActor* Instigator, FName ActionName)
{
	for(UGAction* Action : Actions)
	{
		if(Action && Action->ActionName==ActionName)
		{
			if(!Action->CanStart(Instigator))
			{
				return false;
			}
			// after checking conditions, if client then do server RPC
			if(!GetOwner()->HasAuthority())
			{
				ServerStartAction(Instigator,ActionName);
			}
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
			if(Action->IsRunning())
			{
				Action->StopAction(Instigator);
				return true;
			}
		}
	}
	return false;
}

UGActionComponent* UGActionComponent::GetActionComponent(AActor* Actor)
{
	return Cast<UGActionComponent>(Actor->GetComponentByClass(UGActionComponent::StaticClass()));
}

void UGActionComponent::ServerStartAction_Implementation(AActor* Instigator, FName ActionName)
{
	StartActionByName(Instigator,ActionName);
}

void UGActionComponent::ServerStopAction_Implementation(AActor* Instigator, FName ActionName)
{
	
}
```
