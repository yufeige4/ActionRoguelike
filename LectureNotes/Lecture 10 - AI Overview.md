<a name="s5D9R"></a>
# Unreal AI System
Tips:

- 需要添加"AIModule"至build文件中
- 当build时遇到unresolved external symbol这个问题的时候，可以通过查找缺少的类文件在哪个路径下，再将该模块添加至build中即可修复
<a name="zflPh"></a>
## Navigation Mesh
提供AI可以移动区域的数据
<a name="dTgG1"></a>
## Behavior Tree
AI的大脑，从上往下从左至右执行
<a name="Ue0eS"></a>
### Node

- Selector - 选择一个子节点 (从左至右分析)
   - 检测Decorators是否允许执行
- Sequence - 从左至右顺序执行子节点
   - 当任何'failure'发生时取消sequence

- Task - 行为
- Service - 对于后台任务重复执行
- Decorator - 条件检测，执行流控制
   - Blackboard Based Condition - 检查黑板的基础键值是否满足条件
      - Observer aborts
         - Self - 不满足条件时，将该节点和其子树全部取消执行
         - Low Priority - 当条件满足时，取消执行其他节点而立即执行该节点
         - Both - 结合上述2点
<a name="t0YVu"></a>
## Blackboard
AI的存储数据的地方
<a name="pkUDu"></a>
## Environment Query System
基于测试的对于物体/地点的空间查询
<a name="khnOZ"></a>
## PawnSensing & AIPerception
AI的感知系统
<a name="AvRNv"></a>
## Debugging Tools

- Gameplay Debugger - 游戏内按下'键
- Visual Logger - Window > Developer Tools > Visual Logger
<a name="zksb6"></a>
# 项目代码
GitHub: [https://github.com/yufeige4/ActionRoguelike](https://github.com/yufeige4/ActionRoguelike)<br />Tips: Implements<UObject版的Interface>()，而使用时应使用Interface版的Interface

- 实现简单的范围判定并追击的AI
```cpp
// GBTService_RangeCheck.h

UCLASS()
class ACTIONROGUELIKE_API UGBTService_RangeCheck : public UBTService
{
GENERATED_BODY()

protected:
virtual void TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds) override;

UPROPERTY(EditAnywhere, Category = "AI")
FBlackboardKeySelector Key_TargetActor;

UPROPERTY(EditAnywhere, Category = "AI")
FBlackboardKeySelector Key_AttackRange;

UPROPERTY(EditAnywhere, Category = "AI")
FBlackboardKeySelector Key_InsideAttackRange;

};
```
```cpp
// GBTService_RangeCheck.cpp

void UGBTService_RangeCheck::TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	Super::TickNode(OwnerComp, NodeMemory, DeltaSeconds);

	UBlackboardComponent* MyBB = OwnerComp.GetBlackboardComponent();
	AAIController* MyController = OwnerComp.GetAIOwner();
	
	if(ensure(MyPawn))
		{
			if(TargetActor)
			{
				float AttackRange = MyBB->GetValueAsFloat(Key_AttackRange.SelectedKeyName);
				float Distance = FVector::Distance(TargetActor->GetActorLocation(),MyPawn->GetActorLocation());

				bool InsideAttackRange = Distance <= AttackRange;
				MyBB->SetValueAsBool(Key_InsideAttackRange.SelectedKeyName,InsideAttackRange);
			}
		}
}

```
```cpp
// GAIController.h

class UBehaviorTree;

UCLASS()
class ACTIONROGUELIKE_API AGAIController : public AAIController
{
	GENERATED_BODY()

protected:
	virtual void BeginPlay() override;

	UPROPERTY(EditDefaultsOnly, Category = "AI")
	UBehaviorTree* BehaviorTree;

	virtual void OnPossess(APawn* InPawn) override;
};

```
```cpp
// GAIController.cpp

void AGAIController::BeginPlay()
{
	Super::BeginPlay();
	APawn* PlayerPawn = UGameplayStatics::GetPlayerPawn(this,0);
	UBlackboardComponent* MyBB = GetBlackboardComponent();
	if(ensure(MyBB)&ensure(PlayerPawn))
	{
		MyBB->SetValueAsObject("TargetActor",PlayerPawn);
	}
}

void AGAIController::OnPossess(APawn* InPawn)
{
	Super::OnPossess(InPawn);
	RunBehaviorTree(BehaviorTree);
	
	UBlackboardComponent* MyBB = GetBlackboardComponent();
	

	if(ensure(MyBB))
	{
        // 判断是否实现应判断UObject版
		if(InPawn->Implements<UGAICharacterInterface>())
		{
            // 而使用接口函数时应使用Interface版
			float AttackRange = IGAICharacterInterface::Execute_GetAttackRange(InPawn);
			MyBB->SetValueAsFloat("AttackRange",AttackRange);
		}
	}
}
```
```cpp
// GAICharacterInterface.h

UINTERFACE(MinimalAPI)
class UGAICharacterInterface : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class ACTIONROGUELIKE_API IGAICharacterInterface
{
	GENERATED_BODY()

	// Add interface functions to this class. This is the class that will be inherited to implement this interface.
public:

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
	float GetAttackRange();
	
};
```
