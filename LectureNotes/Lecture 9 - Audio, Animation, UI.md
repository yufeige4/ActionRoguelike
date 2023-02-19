<a name="KeoqF"></a>
# UE Reflection Specifier
- BlueprintNativeEvent - 在C++中实现基础，能够被蓝图扩展
- BlueprintCallable - 允许子类触发
<a name="Jyz69"></a>
## Tips

- C++ const函数可以将函数变成无执行流的蓝图函数
- 使用PostInitializeComponents中实现代理绑定更好
<a name="DjZwF"></a>
# Audio
<a name="wJgsP"></a>
## Sound Cue
<a name="IkWOY"></a>
### 节点

- Wave Player - 播放Sound Wave
- Random - 随机选择Sound Wave并输出
- Modulator - 随机调整音频pitch和volume
- Doppler - 多普勒效应
<a name="cjCdg"></a>
### 其他
Attenuation Setting - 配置音频的Attenuation
<a name="hf9nb"></a>
# Animation Blueprint
<a name="u3uwm"></a>
## Event Graph
基于事件的图表，用于每帧更新ABP和计算AnimGraph中的值
<a name="geCB8"></a>
## Animation Graph
用于采样、混合和操控Pose的图表，并输出最终姿势

- Save Pose - 缓存该Pose
- Use Cached Pose - 使用Pose
- Blend Poses by bool - 使用布尔值控制输出Pose
<a name="jwDM7"></a>
# 项目代码
Github: [https://github.com/yufeige4/ActionRoguelike](https://github.com/yufeige4/ActionRoguelike)<br />Tips：UE4必须在build文件中加入"XAudio2_9"模块才能使用AudioComponent

- 实现火球飞行和击中音效 - Assignment 3
- 实现角色死亡动画逻辑
- 角色死亡基础逻辑
- 实现施法特效 (动画通知) + 火球拖尾 (SpawnEmitterAttached) + 爆炸时相机抖动 - Assignment 3
- 实现角色受伤闪烁 - Assignment 3
- 实现伤害字体显示 + 动画 - Assignment 3
- 统一交互刷新物体基类
- 交互刷新治疗药水 + 音效 + 特效 - Assignment 3

Note: UE4 C++ Interface继承时需继承的时I开头的Interface而不是U开头的Interface !!!<br />![image.png](https://cdn.nlark.com/yuque/0/2023/png/32753014/1676707623923-ad798707-0bd1-403f-97a4-28c547972638.png#averageHue=%23282828&clientId=u506c947c-1a65-4&from=paste&height=299&id=u56a027b9&name=image.png&originHeight=411&originWidth=1040&originalType=binary&ratio=1.375&rotation=0&showTitle=false&size=52527&status=done&style=none&taskId=uf9f6c6a9-fbfd-4f3c-a5c4-a30a719c516&title=&width=756.3636363636364)<br />![image.png](https://cdn.nlark.com/yuque/0/2023/png/32753014/1676707638783-e36e6f8a-c514-413e-b6ea-027c7577f93d.png#averageHue=%23292929&clientId=u506c947c-1a65-4&from=paste&height=279&id=ufa0ab94e&name=image.png&originHeight=384&originWidth=1058&originalType=binary&ratio=1.375&rotation=0&showTitle=false&size=74522&status=done&style=none&taskId=u661bf3f6-ffd7-4eab-81ac-eba28fc4388&title=&width=769.4545454545455)<br />![image.png](https://cdn.nlark.com/yuque/0/2023/png/32753014/1676797725363-b08df8d3-7deb-4106-8c96-83cb6e35f654.png#averageHue=%232a2929&clientId=u651d598c-87f5-4&from=paste&height=657&id=udad1df31&name=image.png&originHeight=904&originWidth=956&originalType=binary&ratio=1.375&rotation=0&showTitle=false&size=193253&status=done&style=none&taskId=u29ab0fa1-22d8-4dac-a193-af96afb8832&title=&width=695.2727272727273)<br />![image.png](https://cdn.nlark.com/yuque/0/2023/png/32753014/1676800805377-3ef1a41c-33b6-4ec6-8a1e-ef6baf06a846.png#averageHue=%23343333&clientId=u651d598c-87f5-4&from=paste&height=687&id=u16302edf&name=image.png&originHeight=945&originWidth=1525&originalType=binary&ratio=1.375&rotation=0&showTitle=false&size=319495&status=done&style=none&taskId=u2e1d83d3-9cdb-4bd2-8072-a01b401aebb&title=&width=1109.090909090909)
```cpp
// GProjectileBase.h
// ...
protected:
	// 飞行音效
	UPROPERTY(VisibleAnywhere, Category = "Components")
	UAudioComponent* AudioComp;

public:
	virtual void BeginPlay() override;

```
```cpp
// GProjectileBase.cpp
// ...
void AGProjectileBase::BeginPlay()
{
	Super::BeginPlay();
	AudioComp->Play();
}
```
```cpp
// GMagicProjectile.h
// ...
	UPROPERTY(EditDefaultsOnly, Category = "Basic")
	float TimeToSelfDestroy;

	UPROPERTY(EditDefaultsOnly, Category = "Audio")
	USoundCue* ImpactSound;

	UPROPERTY(EditDefaultsOnly, Category = "Effects")
	TSubclassOf<UCameraShakeBase> ImpactShake;

	FTimerHandle TimerHandle_selfDestroy;

	void SelfDestroy();

	virtual void Explode_Implementation() override;
```
```cpp
// GMagicProjectile.cpp
// ...
void AGMagicProjectile::BeginPlay()
{
	Super::BeginPlay();
	GetWorldTimerManager().SetTimer(TimerHandle_selfDestroy,this,&AGMagicProjectile::SelfDestroy,TimeToSelfDestroy);
}


void AGMagicProjectile::SelfDestroy()
{
	GetWorldTimerManager().ClearTimer(TimerHandle_selfDestroy);
	if(GEngine)
	{
		GEngine->AddOnScreenDebugMessage(0, 2, FColor::Black, TEXT("SelfDestroyed!"));
	}
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
```
```cpp
// GAttributeComponent.h
// ...
protected:
	// 最大生命值
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Attributes")
	float MaxHealth;

public:
	UFUNCTION(BlueprintCallable)
	bool IsAlive() const;
```
```cpp
// GAttributeComponent.cpp
// ...
UGAttributeComponent::UGAttributeComponent()
{
	CurrHealth = 100.0f;
	MaxHealth = 100.0f;
}

bool UGAttributeComponent::ApplyHealthChange(float Delta)
{
	CurrHealth = FMath::Clamp(CurrHealth+Delta,0.0f,MaxHealth);
	// 调用代理
	OnHealthChanged.Broadcast(nullptr,this,CurrHealth,Delta);
	
	return true;
}

bool UGAttributeComponent::IsAlive() const
{
	return Health > 0.0f;
}
```
```cpp
// GCharacter.h
// ...
protected:

	void DamagedFlash();

    UFUNCTION()
	void OnHealthChanged(AActor* InstigatorActor, UGAttributeComponent* OwningComp, float NewHealth, float Delta);
		
```
```cpp
// GCharacter.cpp
// ...
void AGCharacter::OnHealthChanged(AActor* InstigatorActor, UGAttributeComponent* OwningComp, float NewHealth, float Delta)
{
    if(Delta<0.0f)
	{
		DamagedFlash();
	}
	if(NewHealth<=0.0f && Delta<0.0f)
	{
		APlayerController* PC = Cast<APlayerController>(GetController());
		DisableInput(PC);
	}
}
```
其余代码见Github。
