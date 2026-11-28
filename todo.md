## zidian Todo
- [x] 纹理渲染添加spriteSheet 方便批量的纹理绘制 
- [x] 基础图元绘制贝塞尔曲线 
- [x] 各条渲染管线transform matrix加入 
- [ ] 纹理渲染flip功能
- [ ] SDF渲染管线的引入 
- [ ] 文字Render 
- [ ] 演示Demo开发 flapbird pingpong (可选)
- [ ] 物理模块(可延后)
- [ ] 存档系统 引擎持久化模块
- [ ] 场景管理 实现loading --> main scene场景加载流程
  
## 尺规作图游戏开发 todo
- [x] 游戏技术预研 数学概念 代数数(Algebraic Number)
- [x] 几何谓词 学习
- [ ] 开发第一个可绘制圆形 + 直线的场景
- [ ] 关卡谜题设计 

## 尺规游戏关卡收集
1. 连接两个点 绘制一条直线
2. 绘制一个圆
3. 复制一个指定角
4. 绘制指定长度的一条线段
5. 2等分一个给定角
6. 二等分一条线段
7. 过指定点绘制一条与已知直线平行的直线
8. 三等分一条线段
9. 绘制根号2长度的线段


### canvas状态更新方案
修改顶点属性 新增transformIndex  新增SSBO 存储变换矩阵 创建多条pipeline共用的 global descriptorSet

### 学习使用抓帧工具 RenderDoc

### 学习使用GPU性能分析工具 GPU Profile
Tracy通用分析工具 
硬件级的测量需要不同显卡厂商提供的对应工具

### Vulkan自带GPU Timestamp Query使用 

```C
if(IS_DEBUG){
    PFN_vkSetDebugUtilsObjectNameEXT vkSetDebugUtilsObjectNameEXT =
    reinterpret_cast<PFN_vkSetDebugUtilsObjectNameEXT>(
        vkGetDeviceProcAddr(
            device,
            "vkSetDebugUtilsObjectNameEXT"
        )
    );

    VkDebugUtilsObjectNameInfoEXT nameInfo{};
    nameInfo.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_OBJECT_NAME_INFO_EXT;
    nameInfo.objectType = VK_OBJECT_TYPE_DEVICE;
    nameInfo.objectHandle = reinterpret_cast<uint64_t>(device);
    nameInfo.pObjectName = "Zidian_Global_LogicDevice";
    vkSetDebugUtilsObjectNameEXT(device, &nameInfo);
}




