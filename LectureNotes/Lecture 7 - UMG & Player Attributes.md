<a name="zXl0v"></a>
# UE4 - UPROPERTY Specifier
- EditAnywhere - 可以在蓝图编辑器中和地图中的每个实例进行修改
- VisibleAnywhere - 在蓝图编辑器中和地图中只读
- EditDefaultOnly - 实例中隐藏，蓝图编辑器中可修改
- VisibleDefaultOnly - 变量只读，且仅在蓝图编辑器中 (不常用)

- BlueprintReadOnly - 蓝图脚本只读 (不影响details面板)
- BlueprintReadWrite - 蓝图脚本读写

- Category = "" - 蓝图中的分类显示
<a name="yqZDJ"></a>
# UMG
<a name="HWz74"></a>
## UE4 "STRING" Types

- FString
   - 通常用于Debug
   - 支持string操作
- FName
   - Hashed从而能够快速比较string
   - system和gameplay使用
   - assigned之后无法改变
- FText
   - UI显示的前端文本
   - 更容易地本地化成为不同语言
<a name="nZjSZ"></a>
## Binding

- 每一帧运行
- 性能开销大
<a name="DeECB"></a>
## Event-Based

- 触发时才调用
- 可以自定义触发表现
- C++中添加代理
<a name="CVbyc"></a>
# Construct vs. Pre-Construct
Construct在Widget添加到视口中时才会调用，而Pre-Construct每一次编译都会调用
<a name="OVmKc"></a>
# Tips

- 组件添加到使用的类中，默认规范要求添加 Category = "Components"
- 不建议在变量变化时调用更新UI的函数，而是使用UI单方面监听变量的变化，避免双向调用
- 当制作UI动画时不小心修改了默认值，可以点击动画上的Action->Restore Pre-Animated State查看并恢复
<a name="NhH6i"></a>
# 项目代码
GitHub: [https://github.com/yufeige4/ActionRoguelike](https://github.com/yufeige4/ActionRoguelike)

- 实现属性组件
- 实现魔法弹伤害功能
- 血条Widget显示
- 生命值变化表现

![image.png](https://cdn.nlark.com/yuque/0/2023/png/32753014/1676077248504-e0aeeb7a-7979-4773-bd6b-a7d206753286.png#averageHue=%232a2929&clientId=u76f189c2-d22f-4&from=paste&height=532&id=ua04213fe&name=image.png&originHeight=731&originWidth=1474&originalType=binary&ratio=1.375&rotation=0&showTitle=false&size=219971&status=done&style=none&taskId=u984a46bb-fc99-4bbd-926b-0093b185296&title=&width=1072)
```
// 代理声明
DECLARE_DYNAMIC_MULTICAST_DELEGATE_FourParams(FOnHealthChanged, AActor*, InstigatorActor, UGAttributeComponent*, OwningComp, float, NewHealth, float, Delta);

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class ACTIONROGUELIKE_API UGAttributeComponent : public UActorComponent
{
	GENERATED_BODY()

protected:
	// 生命值
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Attributes")
	float Health;

public:	
	// 对生命值进行修改
	UFUNCTION(BlueprintCallable, Category = "Attributes")
	bool ApplyHealthChange(float Delta);
	// 包含代理
	UPROPERTY(BlueprintAssignable)
	FOnHealthChanged OnHealthChanged;

}
```
```
UGAttributeComponent::UGAttributeComponent()
{
	Health = 100;
}

bool UGAttributeComponent::ApplyHealthChange(float Delta)
{
	Health += Delta;
	// 调用代理
	OnHealthChanged.Broadcast(nullptr,this,Health,Delta);
	return true;
}
```
```
protected:

	// ...

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	UGAttributeComponent* AttributeComp;
```
```
AGCharacter::AGCharacter(){

	// ...
	// 添加属性组件
	AttributeComp = CreateDefaultSubobject<UGAttributeComponent>("AttributeComp");
}
```
```
AGMagicProjectile::AGMagicProjectile()
{
	// ...

	SphereComp->OnComponentBeginOverlap.AddDynamic(this,&AGMagicProjectile::OnActorOverlap);
	DamageAmount = 20.0f;
}

void AGMagicProjectile::OnActorOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if(OtherActor && OtherActor!=GetInstigator())
	{
		UGAttributeComponent* AttributeComp = Cast<UGAttributeComponent> (OtherActor->GetComponentByClass(UGAttributeComponent::StaticClass()));
		if(AttributeComp)
		{
			AttributeComp->ApplyHealthChange(-DamageAmount);
		}
		Explode();
	}
}
```
