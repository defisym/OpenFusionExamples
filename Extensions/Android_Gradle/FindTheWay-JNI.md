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

## 构建

构建时把本仓库的相对目录结构保持完整。CMake 默认引用
`Extensions/_DeLib`；若导出目录结构不同，可通过 `FTW_CORE_INCLUDE` 指向共享头文件目录，
`_3rdLib` 通过共享 helper 的 CMake 接口加入搜索路径。Android 默认使用发布包携带的目标 ABI iconv。

`app/build.gradle` 和 `release.gradle` 都配置 externalNativeBuild。默认 NDK r28c
`28.2.13676358`、CMake `3.22.1`、C++20、共享 libc++，并配置 16 KB 页对齐。
可以用 `-PFTW_NDK_VERSION=...` 选择另一套兼容 NDK。最终导出必须同时包含对应 ABI 的
`libfindtheway.so` 和 `libc++_shared.so`。不要把主机 Linux `.so` 放进 Android 包。

已完成 Android 工具链与真实 Fusion Runtime 的联合构建、四种 ABI 打包、APK 签名及 zipalign 检查。
尚未完成设备/模拟器、实际游戏、真实对象选择/创建和背景碰撞运行验证。

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

运行 `Extensions/_Script/FindTheWay.bat`，由 Python 创建 Android Runtime 源码包并复制到
FindTheWay 的 ToInstall 目录。`FusionRoot` 可通过环境变量覆盖默认安装路径；
未找到 Fusion 安装目录时仍生成 ToInstall 包。脚本不再自动终止本机 JDK 进程。

Android 包包含 Java 适配器、JNI 源码、CMake 模块、ProGuard 规则，以及
`app/src/main/cpp/shared/_DeLib` / `_3rdLib` 中所需的共享头文件。CMake 优先使用包内头文件，
保留 `FTW_CORE_INCLUDE` 显式覆盖能力。包中不包含测试、Fusion SDK 或主机编译产物；包含经过校验的 Android 目标 iconv。

这是源码打包，不是 Android ABI 编译：目标 ABI 的 iconv 已随包提供；NDK/CMake 和真实 Fusion Runtime
仍由 Fusion 构建环境提供。此脚本不会把 Linux `.so` 或 Windows SDK 打包进去。

维护约定：以后修改 FindTheWay Java/JNI、共享头依赖或构建配置时，必须同步检查和更新
`Extensions/_Script/FindTheWay.bat`，并验证暂存包的头文件依赖及独立 CMake 构建。

## Android 目标编译记录

已用 JDK 21、Gradle 8.14.3、AGP 8.11.1、compileSdk 35、NDK r28c 和 CMake 3.22.1，
按批处理生成的源码包布局，与私有 Fusion Runtime 联合执行 `:app:assembleDebug` 成功。
APK 内含 arm64-v8a、armeabi-v7a、x86、x86_64 的 findtheway 和共享 libc++。
独立 Release 原生库 ELF LOAD 段对齐为 16 KB；Debug APK 签名及 zipalign 检查通过。
测试工程只补了应用占位图标，没有实际游戏数据，未做设备/模拟器运行验收。
私有 Runtime/SDK 文件不随公开代码提交。

构建时显式提供每个 ABI 的 iconv：

```text
<iconv-root>/<ABI>/include/iconv.h
<iconv-root>/<ABI>/lib/libiconv.a
```

开发者如需覆盖内置依赖，可传入 `-PFTW_ICONV_ROOT=/path/to/iconv-root`；CMake 自动按 ANDROID_ABI 选择依赖，
缺少相应头文件或库时直接报错。不要使用主机 Linux 的 iconv 库。
静态链接 libiconv 的发行需同时安排其许可证、对应源码及重链接材料；目标 iconv 预编译库、许可证及源码获取校验信息已随工程维护；发布器将对应源码自动纳入 ZIP。

此次联合构建修复了 AndroidCommon.java 漏打包、图片句柄的 int 类型，以及 Gradle namespace、
旧 dexOptions、manifest excluded 参数和可选 Play Core 版本配置；后续维护继续同步批处理。

## 一键发布（iconv 修复）

运行 `Extensions/_Script/FindTheWay.bat` 即可；它调用同目录的 FindTheWay.py，要求 Python 3.10+，
只使用标准库，不需要 pip、7-Zip、额外 iconv 参数或 IDE 工程。准备依赖失败会停住窗口，完整日志在
`Android_Gradle/FindTheWay-package.log`。原有 HTML5 与 ToInstall/综合发布包仍由同一入口生成。

发布器复制完整 native 源码目录和 CMake 模块目录，递归收集引用的共享 C++ 头文件、项目 Java 导入
及同包类引用，因而新增 .cpp/.c/本地头文件不再依赖手工补一个复制条目。
项目新增 native 编译单元仍需在 CMakeLists.txt 中声明，这与是否打包是不同职责。

四个 ABI 的目标 libiconv.a / iconv.h 已固定在 `_3rdLib/iconv-android`，发布前校验 SHA-256。
对应开源源码按固定提交自动下载、校验并缓存，再与许可证及构建说明一起进入 ZIP。
Fusion 默认构建从 ZIP 内找到 iconv，不再需要 `-PFTW_ICONV_ROOT`；该参数仅作为开发者覆盖选项。
首次发布需要网络下载对应源码；下载或文件校验失败时不会生成新的不完整发布包。

此处的一键是依赖准备/发布的一键；Fusion 原有的 JDK、Android SDK/NDK 构建环境仍由 Fusion 配置管理。
测试以无外部 iconv 参数的新目录构建，并检查默认依赖路径实际指向包内。

默认构建复验已通过：全新临时工程不含 FTW_ICONV_ROOT 属性，也没有传入该命令行参数，
四种 ABI 的 CMakeCache 均指向解压包内 iconv；真实 Runtime 的 assembleDebug 成功。
发布器另有 6 项无 SDK 单元检查，覆盖新 native 源文件、Java/头文件依赖、缺库、篡改及下载校验。
