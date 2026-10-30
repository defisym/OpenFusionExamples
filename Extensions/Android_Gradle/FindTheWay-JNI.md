# FindTheWay Android JNI 迁移草案

架构：Fusion Java ACE 分发 → JNI 参数/结果封送 → 同一份 `_DeLib/FindTheWay.h`、`_DeLib/AOE.h`。

寻路、地图代价、直线/移动/攻击范围、ZOC、AOE、等距坐标和缓存均执行 C++。Java 的
`FindTheWayClass`、`AOEClass` 只保留封送和资源管理；对象选择、对象创建、Fusion 参数和事件
仍使用移动 Runtime 的 Java API。共享核心继续保持 Header Only，不复制出 Android 算法分支。

## 审核状态

此草案按共享 helper、核心修正、MapData 和 Android JNI 组织审核。
共享 C++ 的行为改动会影响 Windows，需作者审核后再合并。主机验证、四种 Android ABI 编译及真实 Fusion Runtime 的 Debug APK 联合构建已完成；
尚未完成设备/模拟器运行和实际游戏验收。

共享 helper 的平台差异集中在 `GeneralDefinition.h`，由 `DeLib::Helpers` 提供包含路径和
非 Windows 的 iconv 依赖；寻路代码继续调用原 helper。详见
[GeneralDefinition-Portability.md](../_DeLib/GeneralDefinition-Portability.md)。

## Fusion 接口

所有公开编号与 `Extensions/FindTheWay/Main.h` 一致：29 个条件、21 个动作、20 个表达式。
新增条件 28（OnIterateMap）和动作 20（IterateMap）；沿用新版 C++ 的等距网格打包：
低 16 位为宽，高 16 位为高。地图和路径迭代先取得坐标快照，事件可嵌套执行；路径迭代
更新步索引，地图迭代只更新坐标，与 C++ 一致。

条件 12（图片文件建图）、26（Active 纹理建图）、27（桌面 surface 指针建图）明确消费
参数、释放旧地图并返回 false。没有伪造成功结果。条件 2 的事件驱动碰撞建图仍可用；直接
调用移动 Runtime 碰撞接口的分支保留，必须在实际 Runtime 中验证。背景生成、对象选择与
对象创建也保留现有 Java 实现，主机 API fixture 不验证这些 SDK 行为。

## 生命周期与回调

每个地图由一个 opaque `long` ID 标识。JNI 注册表持有 C++ `shared_ptr`，每次调用也暂时
持有所有权；嵌套事件即使释放地图，也不会让当前 native 调用访问已释放对象。
Java `close()` 幂等，扩展重新建图及销毁时显式调用。没有跨调用保存 `JNIEnv*` 或 Java owner
全局引用，回调仅在当前 Fusion 调用线程同步执行。

JNI 回调名被 ProGuard keep 规则保留。Java callback 异常保持原异常穿过 JNI；原生异常转换为
Java 异常，不跨 JNI ABI 抛出 C++ 异常。字符串按 UTF-16/wchar_t 显式转换，路径名支持中文、
补充平面字符和嵌入 NUL，不依赖 `NewStringUTF` 的编码差异。

背景网格生成将格子中心直接作为图片热点锚点交给 Runtime，热点由 Runtime 应用；
不再通过 DEBUG 开关启用/关闭额外补偿。右边缘、下边缘的业务偏移及地图边界规则保持不变。

## 构建

### Visual Studio 一键打包

构建 `FindTheWay.vcxproj` 的 Run_Only Unicode 配置后，
项目会自动调用 `Extensions/_Script/FindTheWay.bat`，生成
`Extensions/FindTheWay/ToInstall/Files/Data/Runtime/Android/FindTheWay.zip`，并按发布器规则安装、归档。
需要 Python 3.10+；首次运行需要网络下载并校验 iconv 对应源码。
Release Unicode 只生成 Windows 编辑器扩展，Debug Unicode 也不自动打包 Android。
RunOnly 即使 Windows 输出已是最新，点击构建也会重新打包 Android 输入。
打包失败会使 VS 构建失败，详细错误可查看“输出”窗口及
`Extensions/Android_Gradle/FindTheWay-package.log`，不会暂停等待键盘输入。
只构建 Windows 时可用 MSBuild 属性 `/p:FindTheWayPackageAndroid=false` 关闭这一步。

这里在发布阶段就编译四 ABI 的 JNI 库，Fusion 导出 APK 时只负责 Java 编译、资源和库打包。
发布包不包含 `app/build.gradle`、`settings.gradle` 或默认 ProGuard 文件，也不修改安装目录的
`RuntimeAndroid.zip`。这与 DarkEDIF 预编译原生库的发布方式一致。

开发机需要 Android SDK：依次查找 `ANDROID_HOME`、`ANDROID_SDK_ROOT`，Windows 下再查找
`%LOCALAPPDATA%/Android/Sdk`。非标准安装位置可将 `ANDROID_HOME` 指向 Fusion 配置的 SDK。
脚本使用 NDK r28c `28.2.13676358`，优先 SDK CMake `3.22.1`；没有该版本时，
会查找其他 SDK CMake、Visual Studio 自带的 CMake/Ninja 以及 PATH 中的工具，要求 CMake >= 3.22。
仍缺少工具时会调用该 SDK 的 Command-line Tools / sdkmanager 安装，需已接受 Android SDK 许可证并具备网络访问。
没有 Command-line Tools 或依赖安装失败时明确报错，不会继续发布旧库。

CMake 直接编译共享 C++20 核心，目标 Android API 21、Release 优化、16 KB 页对齐。
每次发布都对全部 ABI 执行配置和增量编译，构建缓存位于 `Android_Gradle/_NativeBuild`。
修改共享头文件后仍只需执行同一个 bat。

## 主机验证

需要 Linux、JDK 17+（输出 Java 8 字节码）、GCC 14+、CMake 3.22+、Python 3。
设置 JAVA_HOME，并让 java/javac/cmake/g++ 在 PATH 中可用，然后运行：

```sh
python3 Extensions/Android_Gradle/tests/run_host_tests.py \
  --build-dir /tmp/findtheway-host --sanitize
```

在审核隔离目录中也可使用 `--core /path/to/proposed/Extensions/_DeLib`，不修改原共享头文件。

测试覆盖：直接 C++ 参考与真实 JNI 的路径、范围、ZOC、全部 AOE 型号和方向、等距坐标结果
比对；200 张随机加权地图与独立 Dijkstra 的比对；严格序列化验证与失败导入保持原地图；
缓存模式、强制寻路、UTF-16 名称、重入/异常回调和 1000 次创建/释放。
Fusion 分发测试仅在 SDK 边界使用 fixture，原 Java 扩展及 JNI/核心均为实际代码。
另审计全部 ACE ID，并用 ASan/UBSan 检查 C++。当前沙箱在 ptrace 下运行，LSan 不可用；
脚本显式关闭泄漏检测，不宣称已通过 native 泄漏检查。

## iOS 后续

iOS 可以直接在 Objective-C++ `.mm` 适配层包含同样的共享头文件，调用 C++ 无需 JNI。
本次未修改 iOS 文件，也未创建第二套核心。后续重点是对齐 iOS Runtime 的 ACE 参数、对象
选择/创建和事件回调，而非再次移植寻路算法。

## Windows 打包脚本

运行 `Extensions/_Script/FindTheWay.bat`，由 Python 准备 iconv、编译 JNI、打包并复制到
FindTheWay 的 ToInstall 目录。`FusionRoot` 可覆盖默认 Fusion 安装路径；未找到安装目录时
仍生成 ToInstall 包。脚本只依赖 Python 标准库，不需要 pip 或 7-Zip。
失败时返回非零，独立运行会暂停；VS 调用不暂停。日志在 `Android_Gradle/FindTheWay-package.log`。

扩展 ZIP 的运行所需内容：

- `app/src/main/java`：Java ACE 适配器和依赖类。
- `app/src/main/jniLibs/<ABI>/libfindtheway.so` 与 `libc++_shared.so`：全部四 ABI。
- `app/libs/FindTheWay-JNI-rules.jar`：`META-INF/proguard` 中的 JNI 名称保留规则，由 R8
  自动读取，不覆盖 Fusion 默认的 ProGuard 配置。
- JNI/CMake、递归收集的共享头文件、iconv 目标库、对应源码及许可证，便于检查和重链接。
- 逐文件 SHA-256 清单。

Fusion 保留自己的默认 Gradle、Material 依赖、资源生成、签名、混淆和 AAB 配置。
没有构建钩子，也无需修改默认 Runtime 模板。多个扩展共享的 libc++ 应使用兼容 NDK 版本。

发布器复制整个 native 源码和 CMake 目录，递归收集共享 C++ 头和自有 Java 依赖。
Java 收集范围仅为显式列出的扩展入口、`_DeLib` 和 `_3rdLib` helper；开发 SDK 中的 Runtime、
Banks、Objects、其他内置扩展等由已安装 Fusion 提供，绝不能随 FTW 包覆盖它们。
新增编译单元须在 CMakeLists.txt 中声明；不再额外维护源文件复制清单。
后续修改源码/依赖时须同步检查 bat/Python 发布流程，并验证 APK 内全部 ABI 的库。

四 ABI 的 libiconv.a / iconv.h 固定在 `_3rdLib/iconv-android`，发布前校验 SHA-256；
头文件兼容 Windows CRLF 检出，静态库按字节校验。对应开源源码自动下载、校验、缓存并进入 ZIP。
首次发布需要网络。源代码和重链接材料随包保留，不使用主机 Linux 库或私有 Fusion SDK 源码。

## Android 目标编译记录

已用 JDK 21、Gradle 8.14.3、AGP 8.11.1、compileSdk 35、NDK r28c 和 CMake 3.22.1，
完成四 ABI 编译和真实 Fusion Runtime Debug APK 联合构建。
以原始 Runtime 的 app/build.gradle 和 settings.gradle 逐字节恢复后的配置，
仅加入 jniLibs 中的 FTW/libc++ 预编译库，APK 构建成功；APK 内四 ABI 的两种库均存在。
Material3 主题的资源链接通过；共享 C++ 核心未因发布方式改变而修改。

尚未在 Windows cmd 中执行新版 NDK 构建入口，未进行设备/模拟器和实际游戏运行验收。
不提交私有 Runtime 模板、SDK 或测试应用源码到公开仓库。

主机发布器回归：

```sh
python3 -m unittest discover -s Extensions/Android_Gradle/tests -p test_publisher.py -v
```
