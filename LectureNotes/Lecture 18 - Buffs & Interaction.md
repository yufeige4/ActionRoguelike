<a name="Tx5E8"></a>
# 基于GameplayTag实现buff系统和交互系统
<a name="lz4pq"></a>
## Tips

- 当直接使用ECollisionChannel作为成员变量时会出现以下错误
   - ![image.png](https://cdn.nlark.com/yuque/0/2023/png/32753014/1682162736672-9625b267-fe3d-4ead-aeb1-d0ad0f4f0874.png#averageHue=%23314048&clientId=u9b8cb369-47cf-4&from=paste&height=49&id=u6e80ab9f&name=image.png&originHeight=67&originWidth=1119&originalType=binary&ratio=1.375&rotation=0&showTitle=false&size=12912&status=done&style=none&taskId=uaeaf1179-128e-47d6-9c57-e8c4608a96c&title=&width=813.8181818181819)
   - 通过使用TEnumAsByte<ECollisionChannel>替代ECollisionChannel可以解决该问题
   - 问题是由于Enum类被UE通过反射封装了UInt8实现，所以无法直接作为成员变量类型
<a name="yTjLW"></a>
## 项目代码
GitHub: [https://github.com/yufeige4/ActionRoguelike](https://github.com/yufeige4/ActionRoguelike)

- 实现BUFF基类
   - 包含持续时间、周期效果、时间结束移除和在添加Action时自动生效等功能
- 蓝图中实现灼烧效果
- 魔法火球中包含ActionEffect
- 对交互组件进行优化 (对玩家能够交互的物体进行提示)
   - 后续可以考虑使用不同的Widget对提示信息进行区分
- 属性中添加愤怒值Rage - Assignment 6
   - 受伤增加愤怒值
   - 火焰风暴需消耗愤怒值才可使用
   - 愤怒值在UI中显示
   - 有上限值MaxRage
- 反伤Buff "Thorns" - Assignment 6
   - 一种反弹部分伤害给攻击者的ActionEffect
   - 反弹整数值伤害
   - 无限持续时间
   - 自己对自身造成的伤害不会反弹
- 当怪物看见玩家时，怪物头上出现提示"!" - Assignment 6
   - 仅当第一次发现玩家时提示，即TargetActor设置为玩家时
   - 具有显示的UI动画
- 给玩家赋予新的能力的交互道具 - Assignment 6
   - 仅当玩家未拥有该道具时可以交互
   - 提供冲刺能力
```cpp
// GActionEffect.h
// BUFF基类
UCLASS()
class ACTIONROGUELIKE_API UGActionEffect : public UGAction
{
	GENERATED_BODY()

protected:

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Effect")
	float Duration;
	// time interval of each time triggers
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Effect")
	float Period;

	FTimerHandle TimerHandle_Period;
	FTimerHandle TimerHandle_Duration;

public:

	UGActionEffect();
	
	virtual void StartAction_Implementation(AActor* Instigator) override;

	virtual void StopAction_Implementation(AActor* Instigator) override;
	
protected:

	UFUNCTION(BlueprintNativeEvent, Category = "Effect")
	void ExecutePeriodEffect(AActor* Instigator);
	
};
```
```cpp
// GActionEffect.cpp

void UGActionEffect::ExecutePeriodEffect_Implementation(AActor* Instigator)
{
	if(DebugActionTag.GetValueOnGameThread() && GEngine)
	{
		FString DebugMsg = GetNameSafe(this) + "'s periodical effect on " + GetNameSafe(GetOwningActionComp()->GetOwner())  +" is triggered!";
		GEngine->AddOnScreenDebugMessage(-1,Duration,FColor::Green,DebugMsg);
	}
}

UGActionEffect::UGActionEffect()
{
	bAutoStart = true;
}

void UGActionEffect::StartAction_Implementation(AActor* Instigator)
{
	Super::StartAction_Implementation(Instigator);

	if(Duration>0.0f)
	{
		FTimerDelegate DurationDelegate;
		DurationDelegate.BindUFunction(this,"StopAction",Instigator);
		GetWorld()->GetTimerManager().SetTimer(TimerHandle_Duration,DurationDelegate,Duration,false);
	}
	if(Period>0.0f)
	{
		FTimerDelegate PeriodDelegate;
		PeriodDelegate.BindUFunction(this,"ExecutePeriodEffect",Instigator);
		GetWorld()->GetTimerManager().SetTimer(TimerHandle_Period,PeriodDelegate,Period,true);
	}
}

void UGActionEffect::StopAction_Implementation(AActor* Instigator)
{
	// should not miss the last tick trigger of period event
	if(GetWorld()->GetTimerManager().GetTimerRemaining(TimerHandle_Period)< KINDA_SMALL_NUMBER)
	{
		ExecutePeriodEffect(Instigator);
	}
	
	Super::StopAction_Implementation(Instigator);
	// stop all the timer
	GetWorld()->GetTimerManager().ClearTimer(TimerHandle_Duration);
	GetWorld()->GetTimerManager().ClearTimer(TimerHandle_Period);

	UGActionComponent* ActionComp = GetOwningActionComp();
	if(ActionComp)
	{
		ActionComp->RemoveAction(this);
	}
}
```
```cpp
// GMagicProjectile.h
class UGActionEffect;

UCLASS()
class ACTIONROGUELIKE_API AGMagicProjectile : public AGProjectileBase
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AGMagicProjectile();
	
	virtual void BeginPlay() override;

private:
	
	TArray<AActor*> IgnoreActors;

protected:
	
	UPROPERTY(EditDefaultsOnly, Category = "Damage")
	FGameplayTag ParryTag;
	
	UPROPERTY(EditDefaultsOnly, Category = "Damage")
	float DamageAmount;
	// for applied debuff effect
	UPROPERTY(EditDefaultsOnly, Category = "Damage")
	TSubclassOf<UGActionEffect> AppliedEffect;
	
	UPROPERTY(EditDefaultsOnly, Category = "Basic")
	float TimeToSelfDestroy;

	UPROPERTY(EditDefaultsOnly, Category = "Audio")
	USoundCue* ImpactSound;

	UPROPERTY(EditDefaultsOnly, Category = "Effects")
	TSubclassOf<UCameraShakeBase> ImpactShake;

	FTimerHandle TimerHandle_selfDestroy;

	void SelfDestroy();

	virtual void Explode_Implementation() override;
	
	UFUNCTION()
	void OnActorOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
};
```
```cpp
// GMagicProjectile.cpp
// Sets default values
AGMagicProjectile::AGMagicProjectile()
{
	SphereComp->SetSphereRadius(20.0f);
	SphereComp->OnComponentBeginOverlap.AddDynamic(this,&AGMagicProjectile::OnActorOverlap);
	
	DamageAmount = 20.0f;
	TimeToSelfDestroy = 2.0f;
}

void AGMagicProjectile::BeginPlay()
{
	Super::BeginPlay();
	GetWorldTimerManager().SetTimer(TimerHandle_selfDestroy,this,&AGMagicProjectile::SelfDestroy,TimeToSelfDestroy);
}


void AGMagicProjectile::SelfDestroy()
{
	GetWorldTimerManager().ClearTimer(TimerHandle_selfDestroy);
	/*if(GEngine)
	{
		GEngine->AddOnScreenDebugMessage(0, 2, FColor::Black, TEXT("SelfDestroyed!"));
	}*/
	Destroy();
}

void AGMagicProjectile::Explode_Implementation()
{
	GetWorldTimerManager().ClearTimer(TimerHandle_selfDestroy);
	UGameplayStatics::PlaySoundAtLocation(GetWorld(),ImpactSound,GetActorLocation(),GetActorRotation());
	// 爆炸相机抖动
	UGameplayStatics::PlayWorldCameraShake(GetWorld(),ImpactShake,GetActorLocation(),0.0f,2500.0f);
	Super::Explode_Implementation();
}

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
			if(ActionComp && AppliedEffect)
			{
                // 添加Debuff
				ActionComp->AddAction(AppliedEffect,GetInstigator());
			}
			Explode();
		}
	}
}
```
```cpp
// GInteractionComponent.h

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class ACTIONROGUELIKE_API UGInteractionComponent : public UActorComponent
{
	GENERATED_BODY()

protected:

	UPROPERTY(EditDefaultsOnly, Category = "Trace")
	float TraceDistance;

	UPROPERTY(EditDefaultsOnly, Category = "Trace")
	float TraceRadius;

	UPROPERTY(EditDefaultsOnly, Category = "Trace")
	TEnumAsByte<ECollisionChannel> CollisionChannel;
	
	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = "Interact")
	AActor* FocusedActor;

	UPROPERTY(EditDefaultsOnly, Category = "UI")
	TSubclassOf<UGUserWidget_World> HintWidgetClass;

	UPROPERTY()
	UGUserWidget_World* HintWidgetInstance;

public:	
	// Sets default values for this component's properties
	UGInteractionComponent();
	
	void PrimaryInteract();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

	void FindBestInteractable();

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
		
};
```
```cpp
// GInteractionComponent.cpp

static TAutoConsoleVariable<bool> CVarInteractionDrawDebug(TEXT("ARPG.InteractDrawDebug"),false,TEXT("toggle whether draw debug info for InteractionComp"),ECVF_Cheat);

// Sets default values for this component's properties
UGInteractionComponent::UGInteractionComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	TraceDistance = 500.0f;
	TraceRadius = 30.0f;
	CollisionChannel = ECC_WorldDynamic;
	// ...
}


// Called when the game starts
void UGInteractionComponent::BeginPlay()
{
	Super::BeginPlay();
	 
	// ...
	
}


void UGInteractionComponent::FindBestInteractable()
{
	FCollisionObjectQueryParams ObjectQueryParams;
	ObjectQueryParams.AddObjectTypesToQuery(CollisionChannel);
	
	FVector End;
	
	AActor* MyOwner = GetOwner();
	FVector CameraLocation;
	FRotator CameraRotation;
	
	AGCharacter* MyCharacter = Cast<AGCharacter>(MyOwner);
	MyCharacter->GetCameraViewPoint(CameraLocation,CameraRotation);

	End = CameraLocation + (CameraRotation.Vector()*TraceDistance);
	FCollisionShape Shape;
	Shape.SetSphere(TraceRadius);
	TArray<FHitResult> Hits;
	bool bBlockingHit = GetWorld()->SweepMultiByObjectType(Hits,CameraLocation,End,FQuat::Identity,ObjectQueryParams,Shape);
	// Debug color
	FColor LineColor = bBlockingHit ? FColor::Green : FColor::Red;
	bool bDebugDraw = CVarInteractionDrawDebug.GetValueOnGameThread();

	// 清空之前保存的Focus并重新搜索
	FocusedActor = nullptr;
	
	for(FHitResult Hit : Hits)
	{
		AActor* HitActor = Hit.GetActor();
		if(HitActor!=nullptr)
		{
			// 如果该Actor实现了这个接口
			if(HitActor->Implements<UGGameplayInterface>())
			{
				FocusedActor = HitActor;
				break;
			}
		}
		// Debug Purpose
		if(bDebugDraw)
		{
			DrawDebugSphere(GetWorld(),Hit.ImpactPoint,TraceRadius,32,LineColor,false,2.0f);
		}
	}

	if(FocusedActor)
	{
		// 若当前未实例化过并且widget类被指定
		if(!HintWidgetInstance && ensure(HintWidgetClass))
		{
			HintWidgetInstance = CreateWidget<UGUserWidget_World>(GetWorld(),HintWidgetClass);
		}
		if(HintWidgetInstance)
		{
			// attach到相应的actor上, 如果未在视口中则添加到视口
			HintWidgetInstance->AttachedActor = FocusedActor;
			if(!HintWidgetInstance->IsInViewport())
			{
				HintWidgetInstance->AddToViewport();
			}
		}
	}else
	{
		// 交互物超出范围, 提示消失
		if(HintWidgetInstance && HintWidgetInstance->IsInViewport())
		{
			HintWidgetInstance->RemoveFromParent();
		}
	}
	// Debug purpose
	if(bDebugDraw)
	{
		DrawDebugLine(GetWorld(),CameraLocation,End,LineColor,false,2.0f,0,2.0f);
	}
}

// Called every fame
void UGInteractionComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	FindBestInteractable();
	
}

void UGInteractionComponent::PrimaryInteract()
{
	// refactored, move tracing logic into tick
	if(FocusedActor==nullptr)
	{
		if(CVarInteractionDrawDebug.GetValueOnGameThread())
		{
			GEngine->AddOnScreenDebugMessage(-1,1.0f,FColor::Red,"No FocusedActor to Interact");
		}
		return;
	}
	APawn* MyPawn = Cast<APawn>(GetOwner());
	// 调用接口
	IGGameplayInterface::Execute_Interact(FocusedActor,MyPawn);
}
```
