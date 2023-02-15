<a name="UL5da"></a>
# Basics for Material Editor
<a name="zmF90"></a>
## 输出值

- Base Color - 影响材质的基本颜色
- Metalic - 影响材质的金属度
- Roughness - 影响表面的粗糙 (反射) 程度，越低越类似镜面
- Opacity Mask - 细节面板Blend Mode选择Masked即可使用材质遮罩控制完全显影与否
   - Two Sided - 控制是否能够看见反面
   - Opacity Mask Clip Value - 当 Opacity Mask值小于Clip Value时，该表面不会被渲染
<a name="SOujx"></a>
## 材质节点

- Constant - 固定值
- Parameter - 可调节变量
- Lerp - 插值
- Sine - Sin(input)
- Time - 时间
<a name="G5Mfc"></a>
## 材质函数

- DebugScalarValues
   - ![image.png](https://cdn.nlark.com/yuque/0/2023/png/32753014/1676311635553-b62d3768-2895-4b89-8393-1969c4aaed58.png#averageHue=%232d281e&clientId=u224fac40-833a-4&from=paste&height=108&id=u77589c89&name=image.png&originHeight=148&originWidth=350&originalType=binary&ratio=1.375&rotation=0&showTitle=false&size=42723&status=done&style=none&taskId=uf00bd939-5237-4f83-9fd6-b66e1e906ac&title=&width=254.54545454545453)
   - Result连上Base Color即可很好的可视化输出值
- Expose to Library - 是实现的材质函数能够在菜单中显示
- Use Preview Value as Default - 使用连上的Preview值作为默认值
- LinearGradient - 0到1的渐变
- ![image.png](https://cdn.nlark.com/yuque/0/2023/png/32753014/1676443861673-47d17a28-ae7b-4750-b329-3d9b45e064e2.png#averageHue=%23252523&clientId=u7e133035-4d50-4&from=paste&height=92&id=uff0409ac&name=image.png&originHeight=127&originWidth=237&originalType=binary&ratio=1.375&rotation=0&showTitle=false&size=24193&status=done&style=none&taskId=u9d01b6da-28c4-4539-a9e3-37f1146c100&title=&width=172.36363636363637)
-  If
   - ![image.png](https://cdn.nlark.com/yuque/0/2023/png/32753014/1676443944596-72d110fd-677c-4000-be07-400b435c308d.png#averageHue=%231d1d1b&clientId=u7e133035-4d50-4&from=paste&height=131&id=ucd5da2f7&name=image.png&originHeight=180&originWidth=107&originalType=binary&ratio=1.375&rotation=0&showTitle=false&size=18779&status=done&style=none&taskId=u0639a5ca-9b93-4aa8-889f-8336547fd0a&title=&width=77.81818181818181)
<a name="NJMJH"></a>
## 材质实例
材质的子类

- 仅能调节变量参数
- 无法在原材质的基础上修改功能
- Tips
- 右键材质节点点击Previewing Node
   - 可以很好的看到当前节点的值作为Base Color的效果
- 更高效的可视化，仅编译了这个节点之前的节点
- 蓝图中调节材质的参数 
- ![image.png](https://cdn.nlark.com/yuque/0/2023/png/32753014/1676312234363-5fe2f148-ef5e-4ea1-bf0a-2a030cf8028d.png#averageHue=%23181816&clientId=u224fac40-833a-4&from=paste&height=126&id=uaf643296&name=image.png&originHeight=173&originWidth=284&originalType=binary&ratio=1.375&rotation=0&showTitle=false&size=33817&status=done&style=none&taskId=u8d13442f-0baf-4baf-8a7a-1323444b12d&title=&width=206.54545454545453)
   - 但不如通过材质节点中动态调节更好
      - 蓝图是在CPU中处理的而材质的数据是在GPU中处理的(HLSL)
- 蓝图中的GetTimeSeconds节点等同于材质编辑器中的Time节点
<a name="hPMwv"></a>
# 项目代码
GitHub: [https://github.com/yufeige4/ActionRoguelike](https://github.com/yufeige4/ActionRoguelike)

- 创建HitFlash材质
- 实现受伤闪烁功能
- 实现简单溶解材质
- 实现材质函数HitFlash
- 血条UI材质

![image.png](https://cdn.nlark.com/yuque/0/2023/png/32753014/1676395524228-cc075f92-d3bb-4985-b7f1-5e3c17d4eed5.png#averageHue=%232c2828&clientId=u3306cfa0-8563-4&from=paste&height=420&id=ud0c44928&name=image.png&originHeight=577&originWidth=944&originalType=binary&ratio=1.375&rotation=0&showTitle=false&size=95572&status=done&style=none&taskId=u3ef300d2-d9bb-4fb9-8640-8c6af9e3b18&title=&width=686.5454545454545)
```
public:	
	// Sets default values for this actor's properties
	AGHitFlashingTest();

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	UGAttributeComponent* AttributeComp;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	UStaticMeshComponent* MeshComp;

public:
	UFUNCTION()
	void HitFlash(AActor* InstigatorActor, UGAttributeComponent* OwningComp, float NewHealth, float Delta);

```
```
AGHitFlashingTest::AGHitFlashingTest()
{
	MeshComp = CreateDefaultSubobject<UStaticMeshComponent>("MeshComp");
	RootComponent = MeshComp;
	AttributeComp = CreateDefaultSubobject<UGAttributeComponent>("AttributeComp");

	AttributeComp->OnHealthChanged.AddDynamic(this,&AGHitFlashingTest::HitFlash);
}

void AGHitFlashingTest::HitFlash(AActor* InstigatorActor, UGAttributeComponent* OwningComp, float NewHealth,
	float Delta)
{
	if(Delta<0.0f)
	{
		MeshComp->SetScalarParameterValueOnMaterials("TimeToHit",GetWorld()->TimeSeconds);
	}
}
```

![image.png](https://cdn.nlark.com/yuque/0/2023/png/32753014/1676440567146-9fd524d4-1fe5-4c37-8b8e-1b442ed4fffa.png#averageHue=%232b2b2b&clientId=u7e133035-4d50-4&from=paste&height=436&id=ud77fad8b&name=image.png&originHeight=600&originWidth=988&originalType=binary&ratio=1.375&rotation=0&showTitle=false&size=152752&status=done&style=none&taskId=u3458791f-2016-4a9e-b1e8-f0b9a4e9dd3&title=&width=718.5454545454545)<br />![image.png](https://cdn.nlark.com/yuque/0/2023/png/32753014/1676441262858-25450617-2442-4aa0-87b5-a361efca853a.png#averageHue=%23362726&clientId=u7e133035-4d50-4&from=paste&height=302&id=u8a4c5b2a&name=image.png&originHeight=415&originWidth=1042&originalType=binary&ratio=1.375&rotation=0&showTitle=false&size=103711&status=done&style=none&taskId=u3512d56f-11aa-49d0-bb3d-8de7399340c&title=&width=757.8181818181819)<br />![image.png](https://cdn.nlark.com/yuque/0/2023/png/32753014/1676441440944-b76dc6de-b119-4102-a8dd-17b7b7e4ab7f.png#averageHue=%232a2a29&clientId=u7e133035-4d50-4&from=paste&height=529&id=u6ec9e880&name=image.png&originHeight=727&originWidth=746&originalType=binary&ratio=1.375&rotation=0&showTitle=false&size=99391&status=done&style=none&taskId=uda83b4b5-2a40-4674-bbc5-631882e711b&title=&width=542.5454545454545)<br />![image.png](https://cdn.nlark.com/yuque/0/2023/png/32753014/1676445189235-1b717880-b961-4cc2-9f6f-35f9c5d9162a.png#averageHue=%23312d2b&clientId=u7e133035-4d50-4&from=paste&height=534&id=u7452a8b4&name=image.png&originHeight=734&originWidth=843&originalType=binary&ratio=1.375&rotation=0&showTitle=false&size=93265&status=done&style=none&taskId=u086df290-630f-47ac-bdea-464b2ce14bc&title=&width=613.0909090909091)

