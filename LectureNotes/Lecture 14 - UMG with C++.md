<a name="uktzu"></a>
## UE C++中定义enum类
```cpp
// 在任意.h文件中定义
UENUM(BlueprintType)
enum class EDemo : uint8
{
	A	UMETA(DisplayName = "A"),
	B	UMETA(DisplayName = "B"),
	C	UMETA(DisplayName = "C")
};
```
**黑板中数据（注意C++中的枚举只能通过文本方式添加）！！！**
<a name="cObHz"></a>
## UMG

- NativeTick() - Tick函数
- AddToViewport - C++中的该函数被调用后，随后会调用蓝图中UserWidget的Event Construct函数
<a name="xpurt"></a>
### Meta
<a name="bUUUr"></a>
#### BindWidget
声明该元数据需要在UMG蓝图中designer中被绑定，需要使用相同的变量名称<br />否则将会：![image.png](https://cdn.nlark.com/yuque/0/2023/png/32753014/1679852358302-255614b4-0d0e-4850-aa6c-a68d0a863594.png#averageHue=%232e2c2c&clientId=u13b43982-5213-4&from=paste&height=41&id=u53de359c&name=image.png&originHeight=57&originWidth=830&originalType=binary&ratio=1.375&rotation=0&showTitle=false&size=14423&status=done&style=none&taskId=u086c711f-aec6-41b6-b46b-5b0b708ac35&title=&width=603.6363636363636)
<a name="WrgjU"></a>
### Debug指令
对函数使用UFUNCTION(Exec)宏标记可以让函数能够在游戏中的Console调用<br />仅在如下几个地方中可以声明：

- Player Controller
- 玩家操控的Character
- Game Mode
- Cheat Manager
<a name="kHhto"></a>
## Tips

- 使用UPROPERTY()将变量暴露在UE的反射系统中，即可利用引用计数，能够避免使用野指针
- UMG Data Binding特别费性能，如非必须，尽量避免使用
- GetTimeSeconds函数返回本地时间，多人游戏时无法返回正确的游戏时间
   - 应使用GameState->GetServerWorldTimeSeconds
   - ![image.png](https://cdn.nlark.com/yuque/0/2023/png/32753014/1679864647511-449da697-d1fb-46a8-9f49-644c129affc8.png#averageHue=%23836a28&clientId=u4100b942-f06e-4&from=paste&height=230&id=ud657927c&name=image.png&originHeight=316&originWidth=916&originalType=binary&ratio=1.375&rotation=0&showTitle=false&size=91393&status=done&style=none&taskId=ub3615e8e-b2d2-4a1a-9f91-0d00ab1a6e5&title=&width=666.1818181818181)
- 由于需要支持多语言，所以一定要注意使用FText而不是FString
- Ctrl+P - UE编辑器全局快捷搜东西
- 可以通过加锁的操作让Designer界面中的指定内容无法被选中
- UE的BTService有大坑，按顺序生成死亡的怪物可能共用一个Service实例
   - 需要手动在构造函数中声明**bCreateNodeInstance = true**才能阻止UE为了节省内存的复用
<a name="DZX1m"></a>
## 项目代码
GitHub: [https://github.com/yufeige4/ActionRoguelike](https://github.com/yufeige4/ActionRoguelike)

- 使用代理重构了怪物脱战回血机制，解决了任务没执行完就开始移动的问题 // 参考源码BTTask_MoveTo结点
```cpp
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnRecoverFinished);

UCLASS()
class ACTIONROGUELIKE_API UBTTask_RecoverHealth : public UBTTaskNode
{
	GENERATED_BODY()

	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;

public:

	UBTTask_RecoverHealth();
	
	FOnRecoverFinished OnRecoverFinished;

protected:

	EBTNodeResult::Type TaskState;
	
	bool bStartTask;

	UFUNCTION()
	void FinishedRecover();
};
```
```cpp
EBTNodeResult::Type UBTTask_RecoverHealth::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	if(!bStartTask)
	{
		auto AIC = Cast<AAIController>(OwnerComp.GetOwner());
		if(AIC)
		{
			auto ControlledPawn = AIC->GetPawn();
			if(ControlledPawn && ControlledPawn->Implements<UGAICharacterInterface>())
			{
				IGAICharacterInterface::Execute_Recover(ControlledPawn,this);
				TaskState = EBTNodeResult::InProgress;
				bStartTask = true;
			}
		}
	}
	return TaskState;
}

UBTTask_RecoverHealth::UBTTask_RecoverHealth()
{
	bStartTask = false;
	TaskState = EBTNodeResult::Succeeded;
	OnRecoverFinished.AddDynamic(this,&UBTTask_RecoverHealth::FinishedRecover);
}

void UBTTask_RecoverHealth::FinishedRecover()
{
	UE_LOG(LogTemp,Log,TEXT("Finish Recover!!!"));
	bStartTask = false;
	TaskState = EBTNodeResult::Succeeded;
}
```

- 修复Service检查怪物血量，多只怪物不重置CD问题 // 必须在构造函数中声明**bCreateNodeInstance**以阻止UE复用
- 跟踪窗口显示C++基类
```cpp
UCLASS()
class ACTIONROGUELIKE_API UGUserWidget_World : public UUserWidget
{
	GENERATED_BODY()

protected:
	
	UPROPERTY(meta=(BindWidget))
	USizeBox* ParentSizeBox;

	UPROPERTY(EditAnywhere, Category = "UI")
	FVector WorldDisplayOffset;
	
public:
	
	UPROPERTY(BlueprintReadOnly, Category = "UI")
	AActor* AttachedActor;

protected:
	
	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;
};
```
```cpp
void UGUserWidget_World::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	Super::NativeTick(MyGeometry, InDeltaTime);

	// 判断该Actor是否被回收或即将被回收
	if(!IsValid(AttachedActor))
	{
		// 销毁自身实例
		RemoveFromParent();
		UE_LOG(LogTemp,Warning,TEXT("AttachedActor is destroyed, removing related UserWidget"));
		return;
	}
	
	FVector2D ScreenPosition;
	bool bProjectSuccess = UGameplayStatics::ProjectWorldToScreen(GetOwningPlayer(),AttachedActor->GetActorLocation() + WorldDisplayOffset,ScreenPosition);
	if(bProjectSuccess)
	{
		float ViewportScale = UWidgetLayoutLibrary::GetViewportScale(this);
		ScreenPosition /= ViewportScale;

		if(ParentSizeBox)
		{
			ParentSizeBox->SetRenderTranslation(ScreenPosition);
		}
	}
}

```

- 创建玩家主UI界面
- 怪物血条UI，游戏积分UI，游戏全局信息UI

![image.png](https://cdn.nlark.com/yuque/0/2023/png/32753014/1679854852545-b3eee79b-df0c-4bab-8791-ddb41f74bbf4.png#averageHue=%232a2928&clientId=u13b43982-5213-4&from=paste&height=529&id=u84a15742&name=image.png&originHeight=728&originWidth=1905&originalType=binary&ratio=1.375&rotation=0&showTitle=false&size=275177&status=done&style=none&taskId=u7ffea8cd-259e-4be8-8081-3ade46c0537&title=&width=1385.4545454545455)

- 实现击杀全部怪物和回满玩家血量的作弊指令
