<a name="XclRv"></a>
## EQS
<a name="ZiDBf"></a>
### EQS Testing Pawn
能够设置所需要查看的EQS生成点和效果，对于调试来说非常便利，且仅需拖动即可刷新
<a name="jj2V2"></a>
### Generator
可以通过设置Projection Data将某些生成点映射到附近的NavMesh上
<a name="fGWWg"></a>
### Path Exist Test
可以判断生成点是否有路径可以到达Context，开销大，需谨慎考虑
<a name="gUvM5"></a>
### Game Mode
定义了玩家的类和其Controller的类
<a name="bFev7"></a>
### UCurveFloat
可以通过此类在蓝图中配置曲线
<a name="Q0JRA"></a>
## Tips

- GetAllActorsOfClass函数性能开销很大，将遍历场景中所有的Actor
- 使用TActorIterator遍历场景中的Actor效率高于GetAllActorsOfClass
```cpp
for(TActorIterator<T> It(GetWorld()); It; ++It)
{
	//...
}
```

- C++实现GameMode继承 GameModeBase
<a name="ub1aT"></a>
## 项目代码
GitHub: [https://github.com/yufeige4/ActionRoguelike](https://github.com/yufeige4/ActionRoguelike)

- 实现在玩家周围根据条件刷怪的逻辑
   - EQS判断与玩家距离是否够大和是否存在路径能够到达玩家![image.png](https://cdn.nlark.com/yuque/0/2023/png/32753014/1677838477079-a2794f52-70f8-42ff-be6f-4b36c638d96a.png#averageHue=%233b3a39&clientId=udde6064d-6a9d-4&from=paste&height=407&id=ue1ba484d&name=image.png&originHeight=559&originWidth=2054&originalType=binary&ratio=1.375&rotation=0&showTitle=false&size=123886&status=done&style=none&taskId=u4c86d3b3-b822-4ed1-892b-018ccb77d73&title=&width=1493.8181818181818)
   - GameMode调用
```cpp
// GGameModeBase.h
// ...
UCLASS()
class ACTIONROGUELIKE_API AGGameModeBase : public AGameModeBase
{
	GENERATED_BODY()

public:
	AGGameModeBase();

	virtual void StartPlay() override;

protected:

	UPROPERTY(VisibleAnywhere, Category = "Components")
	UGSpawnControlComponent* SpawnControlComp;
};
```
```cpp
// GGameModeBase.cpp
// ...
AGGameModeBase::AGGameModeBase()
{
	SpawnControlComp = CreateDefaultSubobject<UGSpawnControlComponent>("SpawnControlComp");
}

void AGGameModeBase::StartPlay()
{
	Super::StartPlay();
	SpawnControlComp->SpawnObjects();
}
```

   - 通过组件实现，并挂在GM下 (个人实现方法)
```cpp
// GSpawnControlComponent.h
// ...
UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class ACTIONROGUELIKE_API UGSpawnControlComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UGSpawnControlComponent();

protected:

	UPROPERTY(EditDefaultsOnly, Category = "AI")
	int32 SpawnNumLimit;
	
	UPROPERTY(EditDefaultsOnly, Category = "AI")
	float SpawnInterval;

	UPROPERTY(EditDefaultsOnly, Category = "AI")
	TSubclassOf<AActor> SpawnClass;

	UPROPERTY(EditDefaultsOnly, Category = "AI")
	UEnvQuery* EnvQueryTemplate;

	FTimerHandle TimerHandle_SpawnReset;
	
public:
	UFUNCTION(BlueprintCallable, Category = "AI")
	void SpawnObjects();

	UFUNCTION(BlueprintCallable, Category = "AI")
	void StopSpawning();

protected:
	UFUNCTION()
	void SpawnReset_TimeElapsed();

	UFUNCTION()
	void OnQueryFinished(UEnvQueryInstanceBlueprintWrapper* QueryInstance, EEnvQueryStatus::Type QueryStatus);

};
```
```cpp
// GSpawnControlComponent.cpp
// ...
UGSpawnControlComponent::UGSpawnControlComponent()
{
	SpawnInterval = 5.0;
	SpawnNumLimit = 3;
}

void UGSpawnControlComponent::SpawnReset_TimeElapsed()
{
	int32 CurrNumCount = 0;
	for(TActorIterator<AGAICharacter> It(GetWorld()); It; ++It)
	{
		AGAICharacter* AICharacter = *It;
		if(AICharacter)
		{
			auto AttributeComp = Cast<UGAttributeComponent>(AICharacter->GetComponentByClass(UGAttributeComponent::StaticClass()));
			if(AttributeComp && AttributeComp->IsAlive())
			{
				++CurrNumCount;
			}
		}
	}
	int32 NumLimit = SpawnNumLimit;
	if(DifficultyCurve)
	{
		DifficultyCurve->GetFloatValue(GetWorld()->TimeSeconds);
	}
	if(CurrNumCount>=NumLimit)
	{
		return;
	}
	if(ensureAlwaysMsgf(EnvQueryTemplate,TEXT("EnvQueryTemplate need to be select!!!")))
	{
		auto EQSInstance = UEnvQueryManager::RunEQSQuery(GetWorld(),EnvQueryTemplate,GetOwner(),EEnvQueryRunMode::RandomBest5Pct,nullptr);
		if(EQSInstance)
		{
			EQSInstance->GetOnQueryFinishedEvent().AddDynamic(this,&UGSpawnControlComponent::OnQueryFinished);
		}
	}
}

void UGSpawnControlComponent::SpawnObjects()
{
	GetWorld()->GetTimerManager().SetTimer(TimerHandle_SpawnReset,this,&UGSpawnControlComponent::SpawnReset_TimeElapsed,SpawnInterval,true);
}

void UGSpawnControlComponent::StopSpawning()
{
	GetWorld()->GetTimerManager().ClearTimer(TimerHandle_SpawnReset);
}

void UGSpawnControlComponent::OnQueryFinished(UEnvQueryInstanceBlueprintWrapper* QueryInstance,
                                              EEnvQueryStatus::Type QueryStatus)
{
	if(QueryStatus!=EEnvQueryStatus::Success)
	{
		UE_LOG(LogTemp,Warning,TEXT("Spawn Object EQS Failed!"));
		return;
	}
	
	TArray<FVector> Locations = QueryInstance->GetResultsAsLocations();
	if(Locations.Num()>0)
	{
		FActorSpawnParameters SpawnParams;
		SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;
		if(ensureAlwaysMsgf(SpawnClass,TEXT("SpawnClass need to be select!!!")))
		{
			GetWorld()->SpawnActor<AActor>(SpawnClass,Locations[0],FRotator::ZeroRotator,SpawnParams);
		}
	}
}
```

- 实现怪物低血量逃跑脱战补血功能 - Assignment 4
   - Service - 检测低血量
   - EQS - 找到距离怪物最近的躲藏点
   - Task - 到达隐藏点补满HP
   - 触发间隔为60秒 
