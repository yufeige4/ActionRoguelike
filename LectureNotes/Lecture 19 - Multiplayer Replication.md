<a name="mc0lE"></a>
## Replication
**客户端与服务端同步数据和procedure call的过程**
<a name="IP3Sl"></a>
## Client-Server Model
![image.png](https://cdn.nlark.com/yuque/0/2023/png/32753014/1682466410167-92b0e807-9834-4f46-8e9a-5bf1c5712913.png#averageHue=%23f3a53a&clientId=u06029891-f313-4&from=paste&height=417&id=u23e614a9&originHeight=573&originWidth=825&originalType=binary&ratio=1.375&rotation=0&showTitle=false&size=64845&status=done&style=none&taskId=u25779747-7904-4acb-8b74-855a04ea043&title=&width=600)<br />Note: 数据在服务器和客户端之间传输，无法在客户端之间直传

- **服务器**负责管理游戏状态 // 行为树只在服务器运行
- **客户端**从**服务器**接收数据(Packets)并更新状态
<a name="enVuM"></a>
## Actor Replication
Actor将采用以下两种方式更新:

- Property updates - 自动把更新的变量从**服务器**发送到**客户端**
   - 例子: 玩家血量降低，数据从**服务器**发往所有的**客户端**
- RPCs (Remote Procedure Calls) - 其他**客户端**执行了某个函数
   - 例子: 玩家左键攻击，从**客户端**发RPC到**服务器**
<a name="vutxt"></a>
### Property Replication
变量从**服务器**同步到**客户端**<br />![image.png](https://cdn.nlark.com/yuque/0/2023/png/32753014/1682510142238-8e20b247-e52c-4ab7-a1e9-172c810b8f5b.png#averageHue=%23f0a43a&clientId=u06029891-f313-4&from=paste&height=377&id=ua242d05b&originHeight=518&originWidth=537&originalType=binary&ratio=1.375&rotation=0&showTitle=false&size=42694&status=done&style=none&taskId=ubf0be062-ddcd-4245-8470-847fbfc906d&title=&width=390.54545454545456)
<a name="ia3H0"></a>
### RPC

- Server RPC - **客户端**Call，请求**服务器**执行函数
   - ![image.png](https://cdn.nlark.com/yuque/0/2023/png/32753014/1682518493016-03d46dcd-aef7-477d-956d-31f66efb1a6c.png#averageHue=%23f3a53a&clientId=u06029891-f313-4&from=paste&height=421&id=u35afd717&originHeight=579&originWidth=682&originalType=binary&ratio=1.375&rotation=0&showTitle=false&size=31381&status=done&style=none&taskId=u02b328de-792c-4020-92e5-d9ab1e3da08&title=&width=496) 
- Client RPC - 被**服务器**Call，去某个**特定客户端**上执行
   - ![image.png](https://cdn.nlark.com/yuque/0/2023/png/32753014/1682518720409-05778cbd-49b9-4869-b735-c198eadbd62b.png#averageHue=%23eca139&clientId=u06029891-f313-4&from=paste&height=392&id=ue37d9298&originHeight=539&originWidth=675&originalType=binary&ratio=1.375&rotation=0&showTitle=false&size=44368&status=done&style=none&taskId=u890fe747-ce3b-4c68-b59a-576a77277a1&title=&width=490.90909090909093)
- NetMulticast RPC - 被**服务器**Call，**所有客户端**上执行
   - ![image.png](https://cdn.nlark.com/yuque/0/2023/png/32753014/1682518920609-3ac4d477-75c8-4ca5-9246-84c174f146a2.png#averageHue=%23f5a73a&clientId=u06029891-f313-4&from=paste&height=380&id=uee2f5640&originHeight=522&originWidth=600&originalType=binary&ratio=1.375&rotation=0&showTitle=false&size=43246&status=done&style=none&taskId=uab07620e-9f1a-458e-8a8f-1d44e4d72b0&title=&width=436.3636363636364)
<a name="UvTo1"></a>
## Steps to Implement Multiplayer

- 游玩现有的游戏并观察**客户端**和**服务器**之间game state的不同
   - ![image.png](https://cdn.nlark.com/yuque/0/2023/png/32753014/1682638867397-000f8721-b485-427d-af2d-15a0ca32dc00.png#averageHue=%2338352d&clientId=u7cd507d6-8f2b-4&from=paste&height=339&id=uf8b1179e&originHeight=466&originWidth=346&originalType=binary&ratio=1.375&rotation=0&showTitle=false&size=45255&status=done&style=none&taskId=u2335104a-b623-41ff-b765-a1187bb34df&title=&width=251.63636363636363)
- 将Actor或Component标记为Replicated
- 将需要同步的变量标记为Replicated
- 在代码间添加RPCs
<a name="X5eyD"></a>
## Tips

- Function Specifier
   - Reliable - 保证最终到达. 请求会被重发直到收到ack
   - Unreliable - 不保证到达
- Variable Specifier
   - Replicated - 会被ClientRPC同步
   - ReplicatedUsing = "OnRep_" - RepNotify当变量发生改变时的回调函数
      - 使用该通知时还需在**服务器**手动叫一次RepNotify
<a name="dy5bB"></a>
## 项目代码
GitHub: [https://github.com/yufeige4/ActionRoguelike](https://github.com/yufeige4/ActionRoguelike)

- 将交互组件修复为支持多人游戏
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
	
	UFUNCTION(Server, Unreliable)
	void ServerInteract(AActor* InFocusedActor);
	
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

	auto MyPawn = Cast<APawn>(GetOwner());
	if(MyPawn->IsLocallyControlled())
	{
		FindBestInteractable();
	}
	
}

void UGInteractionComponent::PrimaryInteract()
{
	ServerInteract(FocusedActor);
}

void UGInteractionComponent::ServerInteract_Implementation(AActor* InFocusedActor)
{
	// refactored, move tracing logic into tick
	if(InFocusedActor==nullptr)
	{
		if(CVarInteractionDrawDebug.GetValueOnGameThread())
		{
			GEngine->AddOnScreenDebugMessage(-1,1.0f,FColor::Red,"No FocusedActor to Interact");
		}
		return;
	}
	APawn* MyPawn = Cast<APawn>(GetOwner());
	// 调用接口
	IGGameplayInterface::Execute_Interact(InFocusedActor,MyPawn);
}
```

- 箱子实现了简单的服务器客户端同步
```cpp
// AGItemChest.h
UCLASS()
class ACTIONROGUELIKE_API AGItemChest : public AActor, public IGGameplayInterface
{
	GENERATED_BODY()

protected:

	UPROPERTY(VisibleAnywhere)
	UStaticMeshComponent* BaseMesh;
	
	UPROPERTY(VisibleAnywhere,BlueprintReadOnly)
	UStaticMeshComponent* LidMesh;

	UPROPERTY(ReplicatedUsing = "OnRep_LidOpened") // RepNotify
	bool bLidOpened;
	
public:

	UPROPERTY(EditAnywhere)
	float TargetRoll;
	
	void Interact_Implementation(APawn* InstigatorPawn);

	// Sets default values for this actor's properties
	AGItemChest();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UFUNCTION() // Need to mark UFUNCTION for Unreal
	void OnRep_LidOpened();

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

};
```
```cpp
// AGItemChest.cpp
AGItemChest::AGItemChest()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	BaseMesh = CreateDefaultSubobject<UStaticMeshComponent>("BaseMesh");
	RootComponent = BaseMesh;

	LidMesh = CreateDefaultSubobject<UStaticMeshComponent>("LidMesh");
	LidMesh->SetupAttachment(BaseMesh);

	TargetRoll = -110.0f;

	SetReplicates(true);
}

// Called when the game starts or when spawned
void AGItemChest::BeginPlay()
{
	Super::BeginPlay();
}

void AGItemChest::OnRep_LidOpened()
{
	float CurrPitch = bLidOpened ? TargetRoll : 0.0f;
	LidMesh->SetRelativeRotation(FRotator(0,0,CurrPitch));
}

// Called every frame
void AGItemChest::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void AGItemChest::Interact_Implementation(APawn* InstigatorPawn)
{
	bLidOpened = !bLidOpened;
	OnRep_LidOpened();
}

// key function generated by UHT for replication rules
void AGItemChest::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	// 所有客户端同步bLidOpened变量
	DOREPLIFETIME(AGItemChest,bLidOpened);
}
```
