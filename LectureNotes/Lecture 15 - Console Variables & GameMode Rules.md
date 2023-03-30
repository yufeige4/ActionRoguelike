<a name="TS7cy"></a>
## Game Mode
<a name="FdsZU"></a>
### 获取特殊的GameMode类

- GetWorld()->GetAuthGameMode<T>() - 该函数获取服务端唯一的GM实例，并进行Cast<T>操作
- 等效于Cast<T>(UGameplayStatics::GetGameMode(GetWorld()))
<a name="coc1Y"></a>
## Console Variables
可以通过自定义一些ConsoleVariables来控制程序的执行方式，便于调试，不会在实际打包中出现
```cpp
// 可在CPP文件中进行如下声明
static TAutoConsoleVariable<T> CVarName(TEXT("YourGameName.VarName"),true,TEXT("Hint"),ECVF_Cheat);
CVarName.GetValueOnGameThread() // 获取数据
```
好处：

- 可以通过console命令实现一些调试
- 控制是否Draw Debug信息而不需要临时在代码中Comment或UnComment
<a name="EdPOE"></a>
## Tips

- PlayAnimMontage - 返回的时长为**不考虑播放速度**的蒙太奇动画时长
- 新的设置Timer回调的方法 // 已使用过Lambda表达式和传统传无参数的回调函数两种方法
- UE可以使用类似于C语言Printf的方式打印log
   - UE_LOG(LogTemp,Log,TEXT("%s"),*GetNameSafe(Actor))
```cpp
FTimerHandle TimerHandle_RespawnPlayer;
FTimerDelegate TimerDelegate_RespawnPlayer;
TimerDelegate_RespawnPlayer.BindUFunction(this,"RespawnTimerElapsed" /* 函数名 */,Player->GetController() /* 函数的参数 */);
float RespawnDelay = 2.0f;
		
GetWorldTimerManager().SetTimer(TimerHandle_RespawnPlayer,TimerDelegate_RespawnPlayer,RespawnDelay,false);

// UE可以使用类似于C语言Printf的方式打印log
UE_LOG(LogTemp,Log,TEXT("OnActorKilled: Victim: %s, Killer: %s"),*GetNameSafe(Victim),*GetNameSafe(Killer));
```

- C++函数中的传引用在蓝图中会产生一个return的引脚，而const&则不会
<a name="PwzUO"></a>
## 项目代码
GitHub: [https://github.com/yufeige4/ActionRoguelike](https://github.com/yufeige4/ActionRoguelike)

- 实现玩家死亡重生逻辑
- 修复由于UI创建逻辑写在角色身上导致的BUG，写在PC上并修改了动态绑定的逻辑
   - ![image.png](https://cdn.nlark.com/yuque/0/2023/png/32753014/1679924715332-8b7d92fe-199f-4832-8525-c341bbfea188.png#averageHue=%232b2929&clientId=u673eb642-e683-4&from=paste&height=411&id=u8b017d4b&name=image.png&originHeight=565&originWidth=1246&originalType=binary&ratio=1.375&rotation=0&showTitle=false&size=188342&status=done&style=none&taskId=u82a4187d-601e-4a3f-8ea7-512f1601f1b&title=&width=906.1818181818181)
   - ![image.png](https://cdn.nlark.com/yuque/0/2023/png/32753014/1679924810249-430fa875-4f11-4827-b898-78069ff72420.png#averageHue=%232b2a29&clientId=u673eb642-e683-4&from=paste&height=500&id=u3dd66ee4&name=image.png&originHeight=688&originWidth=1707&originalType=binary&ratio=1.375&rotation=0&showTitle=false&size=246248&status=done&style=none&taskId=u4317114a-ca5a-41ff-a714-dd59dd97e01&title=&width=1241.4545454545455)
- 定义了三个Console Variables
- 实现Gameplay蓝图函数库两种ApplyDamage方法
   - 重构MagicProjectile类
```cpp
// GGameplayFunctionLibrary.h

UENUM(BlueprintType)
enum class EAIState : uint8
{
	InCombat	UMETA(DisplayName = "战斗状态"),
	OutCombat	UMETA(DisplayName = "脱战状态"),
	Died	UMETA(DisplayName = "死亡")
};

UCLASS()
class ACTIONROGUELIKE_API UGGameplayFunctionLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, Category = "Gameplay")
	static bool ApplyDamage(AActor* FromActor, AActor* ToActor, float DamageAmount);

	UFUNCTION(BlueprintCallable, Category = "Gameplay")
	static bool ApplyDirectionalDamage(AActor* FromActor, AActor* ToActor, float DamageAmount, const FHitResult& HitResult);
};
```
```cpp
// GGameplayFunctionLibrary.h

bool UGGameplayFunctionLibrary::ApplyDamage(AActor* FromActor, AActor* ToActor, float DamageAmount)
{
	UGAttributeComponent* AttributeComp = UGAttributeComponent::GetAttributeComponent(ToActor);
	if(AttributeComp)
	{
		return AttributeComp->ApplyHealthChange(FromActor,DamageAmount);
	}
	return false;
}

bool UGGameplayFunctionLibrary::ApplyDirectionalDamage(AActor* FromActor, AActor* ToActor, float DamageAmount,
	const FHitResult& HitResult)
{
	if(ApplyDamage(FromActor,ToActor,DamageAmount))
	{
		auto HitComp = HitResult.GetComponent();
		if(HitComp && HitComp->IsSimulatingPhysics(HitResult.BoneName))
		{
			float ImpulseMagnitude = 300000.0f;
			FVector ImpulseDirection = -HitResult.ImpactNormal;
			HitComp->AddImpulseAtLocation(ImpulseDirection*ImpulseMagnitude,HitResult.ImpactPoint,HitResult.BoneName);
		}
        return true;
	}
	return false;
}
```

- 实现一个积分系统 - Assignment 5
   - 怪物击杀获取积分
   - 使用回复道具消耗积分
   - 拾取金币获取积分
      - 金币为刷新道具的派生类，和回复道具具有相同的基类
- 实现开局自动刷新金币和道具，EQS - Assignment 5 Bonus

Note: 目前MagicProjectile的受击仍有问题，后续将增加MagicProjectile的碰撞通道来正式解决此问题，临时使用修改碰撞属性来暂时修复<br />![image.png](https://cdn.nlark.com/yuque/0/2023/png/32753014/1680161372601-83fa5b89-9c78-4f91-98d9-84c0532c51d8.png#averageHue=%232b3c43&clientId=u4cd3a5a7-18cb-4&from=paste&height=100&id=u2d330637&name=image.png&originHeight=138&originWidth=1093&originalType=binary&ratio=1.375&rotation=0&showTitle=false&size=42301&status=done&style=none&taskId=uc0f1bcc0-bfee-4103-aa3c-a54b543871d&title=&width=794.9090909090909)


