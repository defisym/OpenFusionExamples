---
name: fusion-android-porting
description: 将 Clickteam Fusion 的 Windows/C++ 扩展移植到 Android，或排查 Android Java ACE、JNI、原生库打包、Runtime 版本和坐标绘制问题。适用于 OpenFusionExamples 中以共享 C++ 核心为主、Java 对接 Android Runtime 的扩展；包含单入口发布与 VS RunOnly 集成约定。不用于纯 iOS 或 HTML5 移植任务。
---

# Fusion Android 扩展移植

## 适用边界

仅在任务明确涉及 Fusion Android／JNI 移植、构建、发布或故障排查时使用。
纯 iOS／Objective-C++、HTML5 或 Windows 开发不启用本 skill。
同时涉及多个平台时，只将本流程用于 Android 部分；共享 C++ 的修改仍需考虑其他平台影响。
未来 iOS 流程应建立独立的 `fusion-ios-porting`，不在本 skill 中添加另一套平台构建步骤。

以共享 C++ 为核心，Java 只负责 Fusion Runtime 接口与 JNI 封送。发布时编译原生库，Fusion
导出 APK 时使用自己的默认构建模板。先阅读仓库指令与目标扩展的现有实现，再执行下列流程。

## 先收集事实

1. 确认扩展名、Windows ACE 定义、现有移动端实现、实际安装的 Fusion Runtime 和 SDK 版本。
2. 检查本地改动及目标分支，保留用户未提交内容；不要将开发 SDK 当成待发布的扩展源码。
3. 将 C++ 变化与 Android 适配、构建脚本变化分别列清。共享 C++ 修改交作者审核；未经授权
   不合并主分支。当前任务已有授权时，不重复要求批准常规检查或 Android 修复。
4. 私有 Fusion SDK 只用于本地检查、编译和接口核对，不将其源码、模板或二进制提交到公开仓库。

## 实施顺序

1. **整理接口清单。** 对照 Windows 的条件、动作、表达式 ID、参数顺序、类型、返回值及事件。
   Java 的 switch 分发须保持编号一致。不支持的移动功能仍须消费全部参数，按接口返回失败或空值。
2. **移植公共依赖。** 检查 Header Only 代码是否依赖 PCH、Win32 类型、宏、字符串和路径函数。
   优先修改公共 helper，使用标准库和窄范围平台条件；不要在各调用点复制替代实现。
3. **建立 JNI 边界。** C++ 保留算法和数据模型；Java 处理 Runtime 对象、事件和参数。
   旧数值／存档兼容留在 Fusion 适配层，不写回最新核心模型。参见
   [实施细节](references/workflow.md)。
4. **在开发／发布阶段编译原生库。** 对所有支持 ABI 构建 Release JNI 库、带齐运行依赖，
   放入 `app/src/main/jniLibs/<ABI>/`。不要让普通用户的 Fusion 导出过程重新编译扩展 C++。
5. **收紧包内容。** Java 只收集显式扩展入口及自有 helper。不要打包开发 SDK 的 Runtime、Banks、
   Objects、内置扩展等；不要覆盖 `app/build.gradle`、`settings.gradle` 或默认 ProGuard 文件。
   也不修改安装目录中的 `RuntimeAndroid.zip`。
6. **保持单入口。** 一个 bat 调用 Python 完成工具查找、依赖准备、全 ABI 增量编译、校验、打包、
   安装和归档。失败返回非零并保留日志；独立运行可暂停，VS 调用不得等待键盘。
   本仓库仅 `Run_Only Unicode` 自动发布 Android，Release／Debug 保留 Windows 构建。
7. **逐层验证。** 验证核心 → JNI／ACE → 包内容 → 真实 Runtime APK → 设备显示与交互。
   分清每层证据，不将主机 fixture 或 APK 编译通过写成设备验收通过。

## 坐标和图像专项检查

- 先区分格子索引、格子中心、图片热点、图片左上角、帧／窗口／图层坐标。
- 阅读实际 Runtime 的背景精灵、直接粘贴和重绘路径，确认各路径何时应用热点。
- 多帧素材不能假设热点相同。若 Windows 按参考帧的共同左上角绘制，而 Android 按各帧热点
  绘制，目标帧锚点应为 `参考锚点 + 目标帧热点 - 参考帧热点 + 业务偏移`。
- 测试应比较 Runtime 应用热点后的图片位置；只检查传入的原始锚点会漏掉障碍物旁的边框错位。
  同时覆盖内部障碍、地图外边界、滚动、目标图层及不同热点。不要用 DEBUG 开关改变坐标语义。

## 交付前检查

- 包内仅有目标扩展需要的 Java、目标 ABI 库、扩展资源及依赖／许可材料，没有 Runtime SDK 源码。
- 每种 ABI 都包含扩展 `.so` 及其所需共享库；与其他原生扩展共存时检查 libc++ 版本兼容性。
- JNI 类／回调名字不会被 R8 改写。沿用现有消费规则 JAR 方案，不覆盖应用全局混淆配置。
- 从完整开发 SDK 目录打包也不会混入 SDK；从新暂存目录生成 ZIP，不携带旧产物。
- 变更涉及源码、依赖或构建配置时，同步维护并执行发布入口，不只手工复制文件验证。
- 汇报改动、实际执行的检查、未验证范围和作者需要操作的入口；提交保持逻辑清楚，更新既有 PR。

## 仓库内参考实现

以下路径相对仓库根目录，读取时以当前代码为准，不在 skill 中维护第二份脚本：

- `Extensions/_Script/FindTheWay.bat`、`FindTheWay.py`：单入口发布、工具发现、依赖边界及打包。
- `Extensions/Android_Gradle/app/src/main/cpp/FindTheWayJNI.cpp`：JNI 句柄、封送和回调。
- `Extensions/Android_Gradle/app/src/main/java/Extensions/CRunFindTheWay.java`：ACE 和背景边框对齐。
- `Extensions/Android_Gradle/FindTheWay-JNI.pro`：扩展 JNI 消费规则。
- `Extensions/Android_Gradle/tests/`：核心对照、JNI、ACE、发布器及坐标检查。
- `Extensions/Android_Gradle/FindTheWay-JNI.md`：FTW 的实现状态及构建参数。

实施时阅读 [workflow.md](references/workflow.md)；失败时按
[troubleshooting.md](references/troubleshooting.md) 从最新一次构建定位问题。
本 skill 记录 Android 已实践流程；其他平台的 SDK、打包和验收均需独立核对。
