<a name="ZctUD"></a>
## FGameplayTag
- 将FName TagName封装在其中
- 可代替bool, enum, FName使用
- 可以在项目设置中创建
- 可以在虚幻编辑器中选择
- 被GAS广泛使用
<a name="gWGMA"></a>
## FGameplayTagContainer

- 封装了TArray<FGameplayTag> GameplayTags
- HasTag函数判断容器中是否存在该Tag
   - Exact Match选项
      - 为True时要求准确匹配tag
      - 为False时，可使用非叶子级的Tag作为条件进行判断
<a name="mXg8H"></a>
## 优点

- 不用进行任何类型转换，只需要获取Tag管理组件，通过容器对Tag进行条件判断即可使用
- 减少组件和各种类之间的依赖，解耦
- 更好的支持多人游戏
<a name="GEwzF"></a>
## Tips

- 在类中用到具体类而非指针作为成为变量时，需要#include而不是Forward Declaration
- 除了声明GameplayTag作为成员变量来使用，还可以hardcode成如下形式(强烈不建议，后续难以维护)，如果使用建议声明为静态的
```cpp
static FGameplayTag Tag = FGameplayTag::RequestGameplayTag("TagName");
```
<a name="xeMHP"></a>
## 项目实战
GitHub: [https://github.com/yufeige4/ActionRoguelike](https://github.com/yufeige4/ActionRoguelike)<br />Note: 在ActionComp中而不是Action进行IsRunning判断，能够使子类在继承和重载StartAction或StopAction时不用每次进行IsRunning的判断，减少可能出现BUG的可能性

- Action，ActionComponet引入Tags机制
- 将之前实现的几种Action使用Tag进行互斥处理
- 使用GameplayTag作为判断触发的条件
- 使用GameplayTag实现反弹BUFF
- 给主角添加ActionComponent
- 给AI怪物添加ActionComponent

![image.png](https://cdn.nlark.com/yuque/0/2023/png/32753014/1681414353195-62d23fd8-3f48-4589-9dfd-37deef3c9117.png#averageHue=%23494949&clientId=u5a4c2062-8d62-4&from=paste&height=271&id=VdlCc&name=image.png&originHeight=373&originWidth=1010&originalType=binary&ratio=1.375&rotation=0&showTitle=false&size=28226&status=done&style=none&taskId=u4f494b5c-c85b-4137-b757-3c15ef61dad&title=&width=734.5454545454545)
```cpp
// GAction.h
UCLASS(Blueprintable)
class ACTIONROGUELIKE_API UGAction : public UObject
{
	GENERATED_BODY()

public:
	// nickname of an action
	UPROPERTY(EditDefaultsOnly, Category = "Action")
	FName ActionName;
	
protected:
	// Tags added to owning actor
	UPROPERTY(EditDefaultsOnly, Category = "Tags")
	FGameplayTagContainer GrantedTags;
	// Actions can only start without having these tags
	UPROPERTY(EditDefaultsOnly, Category = "Tags")
	FGameplayTagContainer BlockedTags;
	// internal bool to check whether it is running
	bool bIsRunning;
	
public:
	// functions
	UFUNCTION(BlueprintNativeEvent, Category = "Action")
	bool CanStart(AActor* Instigator);
	
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Action")
	void StartAction(AActor* Instigator);

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Action")
	void StopAction(AActor* Instigator);

	virtual UWorld* GetWorld() const override;

	UFUNCTION(BlueprintCallable, Category = "Action")
	bool IsRunning() const;

protected:
	
	UFUNCTION(BlueprintCallable, Category = "Action")
	UGActionComponent* GetOwningActionComp() const;
};
```
```cpp
// GAction.cpp
bool UGAction::CanStart_Implementation(AActor* Instigator)
{
	if(IsRunning())
	{
		return false;
	}
	UGActionComponent* ActionComp = GetOwningActionComp();
	if(ActionComp && ActionComp->ActiveGameplayTags.HasAny(BlockedTags))
	{
		return false;
	}
	bIsRunning = true;
	return true;
}

void UGAction::StartAction_Implementation(AActor* Instigator)
{
	UE_LOG(LogTemp,Log,TEXT("StartAction: %s"),*GetNameSafe(this));

	ensureAlwaysMsgf(bIsRunning,TEXT("Trying to stop action that was not started!"));
	
	UGActionComponent* ActionComp = GetOwningActionComp();
	if(ActionComp)
	{
		ActionComp->ActiveGameplayTags.AppendTags(GrantedTags);
	}
}

void UGAction::StopAction_Implementation(AActor* Instigator)
{
	UE_LOG(LogTemp,Log,TEXT("StopAction: %s"),*GetNameSafe(this));
	
	UGActionComponent* ActionComp = GetOwningActionComp();
	if(ActionComp)
	{
		ActionComp->ActiveGameplayTags.RemoveTags(GrantedTags);
	}
	bIsRunning = false;
}

UGActionComponent* UGAction::GetOwningActionComp() const
{
	return Cast<UGActionComponent>(GetOuter());
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

bool UGAction::IsRunning() const
{
	return bIsRunning;
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
	void AddAction(TSubclassOf<UGAction> ActionClass);

	UFUNCTION(BlueprintCallable, Category = "Actions")
	bool StartActionByName(AActor* Instigator, FName ActionName);

	UFUNCTION(BlueprintCallable, Category = "Actions")
	bool StopActionByName(AActor* Instigator, FName ActionName);

	UFUNCTION(BlueprintCallable, Category = "Actions")
	static UGActionComponent* GetActionComponent(AActor* Actor);

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

	if(DebugActionTag.GetValueOnGameThread())
	{
		if(GEngine)
		{
			FString DebugMsg = GetNameSafe(GetOwner()) + " : " + ActiveGameplayTags.ToStringSimple();
			GEngine->AddOnScreenDebugMessage(-1,0.0,FColor::White,DebugMsg);
		}
	}
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
			if(!Action->CanStart(Instigator))
			{
				return false;
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
```
```cpp
// GMagicProjectile.h
UCLASS()
class ACTIONROGUELIKE_API AGMagicProjectile : public AGProjectileBase
{
	GENERATED_BODY()

	// ...

protected:

	UPROPERTY(EditDefaultsOnly, Category = "Damage")
	FGameplayTag ParryTag;

};
```
```cpp
// GMagicProjectile.cpp\
// ...
void AGMagicProjectile::OnActorOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
                                       UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if(OtherActor && OtherActor!=GetInstigator())
	{
		UGActionComponent* ActionComp = UGActionComponent::GetActionComponent(OtherActor);
		if(ActionComp && ActionComp->ActiveGameplayTags.HasTag(ParryTag))
		{
			// 识别到反弹BUFF
			MoveComp->Velocity = -MoveComp->Velocity;
			// 改变魔法导弹的所有权
			SetInstigator(Cast<APawn>(OtherActor));
			return;
		}
		
		if(IgnoreActors.Find(OtherActor)!=INDEX_NONE)
		{
			return;
		}
		IgnoreActors.Push(OtherActor);
		if(UGGameplayFunctionLibrary::ApplyDirectionalDamage(GetInstigator(),OtherActor,-DamageAmount,SweepResult))
		{
			Explode();
		}
	}
}
```
![image.png](https://cdn.nlark.com/yuque/0/2023/png/32753014/1681414881885-b15a12b5-c64a-4f31-a818-dbe741090df0.png#averageHue=%232e2e2e&clientId=u5a4c2062-8d62-4&from=paste&height=479&id=u951ca128&name=image.png&originHeight=658&originWidth=1756&originalType=binary&ratio=1.375&rotation=0&showTitle=false&size=125934&status=done&style=none&taskId=u174efa35-177f-4828-854c-80904168f37&title=&width=1277.090909090909)<br />![image.png](https://cdn.nlark.com/yuque/0/2023/png/32753014/1681414904500-c4304b75-0794-4b5c-b6bd-e3dd8bace9cb.png#averageHue=%23292928&clientId=u5a4c2062-8d62-4&from=paste&height=463&id=ue57b3a7d&name=image.png&originHeight=637&originWidth=1146&originalType=binary&ratio=1.375&rotation=0&showTitle=false&size=135533&status=done&style=none&taskId=ud4b4a047-8802-4caa-b959-d914313a0b6&title=&width=833.4545454545455)
