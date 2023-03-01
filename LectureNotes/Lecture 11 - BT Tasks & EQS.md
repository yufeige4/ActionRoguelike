<a name="kiHT8"></a>
## Decorator
- Cooldown - 执行后冷却一定时间并且return false
- Loop - 连续执行n次，但与Cooldown节点不能在一个节点中使用
<a name="rkCb0"></a>
## Service

- Set Default Focus - 设置面朝某物
<a name="dfBXY"></a>
## EQS
Make Spatial Query<br />建议在蓝图中实现，而不是C++
<a name="nCOVz"></a>
### Generator
Querier - 发出查询的物体<br />Center - 以某个QueryContext为中心<br />Arc Direction - 生成的方向
<a name="yw1e5"></a>
#### Tests: 

- Distance: To Querier - 居于距离查询者的距离进行测试
- Trace: To Querier on Visibility - 对Context物体和生成点之间进行射线检测
<a name="JT0oX"></a>
## Pawn Sensing Component
更早于AI Perception Component，更容易用，但是功能更少<br />![image.png](https://cdn.nlark.com/yuque/0/2023/png/32753014/1677684333974-2218fe97-b143-46f2-90b7-daa24469b4e1.png#averageHue=%23323231&clientId=uf805ae7e-ecdf-4&from=paste&height=668&id=uc9f92268&name=image.png&originHeight=919&originWidth=1312&originalType=binary&ratio=1.375&rotation=0&showTitle=false&size=586086&status=done&style=none&taskId=u8e285cec-4d24-46f2-be08-ab9eb401949&title=&width=954.1818181818181)<br />可以通过调节Character类中的Base Eye Height调节眼睛高度，使之与模型匹配<br />![image.png](https://cdn.nlark.com/yuque/0/2023/png/32753014/1677684469506-21bbbe91-4c1a-49b7-9277-62bea4683acf.png#averageHue=%235f5c53&clientId=uf805ae7e-ecdf-4&from=paste&height=126&id=u83343cd8&name=image.png&originHeight=173&originWidth=479&originalType=binary&ratio=1.375&rotation=0&showTitle=false&size=12645&status=done&style=none&taskId=ud68de38e-969f-427c-a1ec-47e764afcef&title=&width=348.3636363636364)

Tips: 

- ensureMsgf(Variable,TEXT("Blah Blah")) - 可以在为空的时候做出提示，更方便从Output Log中发现问题
- 绑代理的回调函数一定要加UFUNCTION反射，不然无法叫到！！！
<a name="H5AEK"></a>
# 项目代码
GitHub: [https://github.com/yufeige4/ActionRoguelike](https://github.com/yufeige4/ActionRoguelike)

- 简单的攻击逻辑
- 在目标周围找到离自身最短距离的点进行攻击
   - 从TargetActor至Querier以120度画扇形，在此区域中生成点![image.png](https://cdn.nlark.com/yuque/0/2023/png/32753014/1677681807162-e0d5f998-c038-487b-b76e-85d6edfb5371.png#averageHue=%233c3c3b&clientId=uf805ae7e-ecdf-4&from=paste&height=345&id=u93af8f64&name=image.png&originHeight=474&originWidth=1771&originalType=binary&ratio=1.375&rotation=0&showTitle=false&size=88858&status=done&style=none&taskId=u21b254cb-0e85-4872-b678-6106a954b9d&title=&width=1288)
   - 根据距离Querier的距离作筛选![image.png](https://cdn.nlark.com/yuque/0/2023/png/32753014/1677682002038-c2cc36cc-350b-42cc-a32b-594b742577e6.png#averageHue=%23197519&clientId=uf805ae7e-ecdf-4&from=paste&height=57&id=u8dcc847f&name=image.png&originHeight=78&originWidth=672&originalType=binary&ratio=1.375&rotation=0&showTitle=false&size=10216&status=done&style=none&taskId=uf47b6a1e-8475-41ac-b189-572f4add492&title=&width=488.72727272727275)
   - 将生成点和TargetActor之间进行Visibility通道的射线检测进行筛选，要求无blocking hit![image.png](https://cdn.nlark.com/yuque/0/2023/png/32753014/1677682382173-86a33912-51cb-46dd-a17f-ac438ac9a4e8.png#averageHue=%23197119&clientId=uf805ae7e-ecdf-4&from=paste&height=70&id=u4b7c28bd&name=image.png&originHeight=96&originWidth=685&originalType=binary&ratio=1.375&rotation=0&showTitle=false&size=17311&status=done&style=none&taskId=u7637c229-f7ea-4240-a91e-47d0c321641&title=&width=498.1818181818182)
- 给怪物添加视觉感知功能
