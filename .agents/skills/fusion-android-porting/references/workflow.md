# 实施细节

## 1. 识别开发 SDK 与实际 Runtime

开发 SDK 用于了解和编译接口；用户安装的 `Data/Runtime/Android/RuntimeAndroid.zip` 是 Fusion
构建工程的基础。两者可能来自不同版本，检查方法签名、图片句柄类型及资源生成任务，不只看文件名。
例如图片句柄可能从 `short` 升级到 `int`，强制窄化不能作为通用修复。

迁移前制作 ACE 对照表：编号、参数顺序、读取 API、返回值、触发事件、暂不支持的功能。
条件／动作／表达式分别核对；不能只编译 Java switch 后就认为接口一致。
移动端无法读取桌面 surface／纹理等功能时，按约定返回空或失败，不伪造成功；明确失败后地图等
资源的状态，保留参数消费顺序和对象生命周期。

## 2. 移植共享 C++ 和 helper

- 补齐头文件自身的标准库 include，检查脱离 Windows PCH 后是否仍能独立编译。
- Windows 专属类型按语义定义项目类型；数据模型与 Fusion 事件数值表示分离。
- `min`／`max` 改用 `(std::min)(a, b)`、`(std::max)(a, b)`，避免 Win32 宏展开。
- 平台差异集中在 helper，保持调用方复用。非 Windows 路径处理可用 `std::filesystem`；
  字符串转换按所需编码提供 iconv 等实现，Windows 可继续使用原 API。
- `CP_ACP` 不是跨平台固定编码；本项目非 Windows 默认 UTF-8，必须可配置并说明语义。
- 不用共享静态字符串承载返回值。路径／转换结果应由调用者或返回的拥有型对象持有。
  兼容 `char*` 的 helper 仍须明确生命周期，避免临时对象析构后的悬空指针和线程间互相覆盖。
- Android NDK 没有公共 iconv API。必须提供目标 ABI 的依赖，不能误链接主机 Linux 库。
- 测试和 CMake 放 Android 构建目录；移植不要求为了构建组织而拆分所有历史 Header Only 文件。

共享 C++ 的行为改动需在 PR 中列出并交作者审核。不要将尚未批准的改动直接合入主分支。

## 3. JNI 接口和生命周期

- 算法只保留在共享 C++。Java 承担 Fusion 对象选择、创建、事件、参数读取及返回值封送。
- Native 对象用可校验句柄和明确所有权管理。FTW 使用 `jlong` 句柄注册表，调用期间保留共享所有权，
  防止事件重入销毁地图后仍继续访问它；Java `close()` 必须幂等，重建和销毁时释放。
- 不跨线程复用 `JNIEnv*`，也不跨调用保留会失效的 local reference。需要长期保留 Java 对象时，
  使用成对释放的 global reference，并考虑线程附着和类加载器。
- Java 字符串按 UTF-16 处理，注意 Windows 与 Android 的 `wchar_t` 宽度不同。
  不用 `NewStringUTF` 冒充任意 Unicode 转换；检查补充平面字符和嵌入 NUL。
- 不让 C++ 异常穿过 JNI ABI；Java callback 的 pending exception 不应被静默吞掉后继续 JNI 调用。
- 检查 Java 声明、JNI 导出／注册名、参数宽度与返回类型。数组和批量封送可降低跨边界开销。
- 为按名字访问的类、native 方法及 Java 回调提供 R8 keep 规则。FTW 将规则放到
  `app/libs/FindTheWay-JNI-rules.jar` 的 `META-INF/proguard/FindTheWay.pro`，由默认依赖流程读取。

## 4. 开发阶段构建原生库

当前 FTW 实践参数，不应当作所有未来 Runtime 永久固定的版本要求：

| 项目 | 已使用配置 |
| --- | --- |
| Python | 3.10+，发布器仅使用标准库 |
| JDK | JDK／JBR 21，供 Fusion／Gradle 构建 APK |
| Gradle / AGP | 8.14.3 / 8.11.1，来自实际 Runtime |
| NDK | r28c，`28.2.13676358` |
| CMake | 优先 SDK 3.22.1，兼容已安装的 >= 3.22 版本 |
| C++ / 原生目标 API | C++20 / Android API 21 |
| ABI | arm64-v8a、armeabi-v7a、x86、x86_64 |

发布器依次查找 `ANDROID_HOME`、`ANDROID_SDK_ROOT`、Windows 的
`%LOCALAPPDATA%/Android/Sdk`。CMake/Ninja 除 SDK 外，也查找 VS 的工具目录和 PATH；
VS CMake 和 Ninja 可能在同一外层目录下的不同子目录，不要假设两个程序总在同一 bin 中。
仍缺少依赖时才调用 SDK 的 sdkmanager；SDK Command-line Tools 自身也可能未安装。
不要把“准备安装”写成“已经可用”，安装后应再次检查工具和产物。

对每个 ABI 配置 CMake、增量编译 Release 库，明确选择 Android toolchain、原生 API、优化和
16 KB 页对齐。编译失败立即停止发布，不回退为上一次的 `.so`。
使用共享 STL 时携带匹配的 `libc++_shared.so`；检查 APK 中所有原生依赖，不只扩展主库。
与其他扩展共存时不能假设不同 NDK 的 libc++ 可任意相互替换。

## 5. 扩展 ZIP 和单入口

Fusion 自动拼装 Runtime 与扩展 ZIP。参照预编译原生扩展的流程：

```text
开发者 VS RunOnly / bat
  → 校验依赖和对应源码
  → 编译各 ABI 的共享 C++ 核心及 JNI
  → 收集扩展 Java、jniLibs、资源、消费规则、许可材料
  → 写入清单、创建 ZIP、安装与归档
Fusion 构建 APK
  → 拼装自己的 Runtime 和扩展 ZIP
  → 默认 Gradle 编译 Java、链接资源、打包原生库及签名
```

Java 收集必须有所有权边界。FTW 的显式入口是 `Extensions/CRunFindTheWay.java`，
自有 helper 位于 `_DeLib` 和 `_3rdLib`。沿 import 递归时，遇到 SDK 提供的 Runtime、Banks、
Objects、内置扩展等应停止，不因为本地恰好存在对应 `.java` 就复制它。
SDK 只读使用；公开包不携带私有 SDK 内容。

包内布局：

```text
app/src/main/java/Extensions/CRun<Extension>.java
app/src/main/java/<自有 helper>/...
app/src/main/jniLibs/<ABI>/lib<extension>.so
app/src/main/jniLibs/<ABI>/libc++_shared.so   # 使用共享 STL 时
app/libs/<extension>-JNI-rules.jar
<依赖许可证、对应源码、重链接材料、文件清单>
```

为了检查和重链接，可随包保留自有 native/CMake 源码及共享头文件，但不要附带 SDK／构建缓存。
逐文件哈希必须考虑 Git 换行转换：固定头文件 LF，兼容已有 CRLF 检出；库、压缩包等二进制仍严格
按字节校验。依赖下载固定版本并核对哈希，遵守许可证的源码和重链接要求。

生成前清理暂存目录，验证全部 ABI 后再以临时文件原子替换输出 ZIP。
不要覆盖默认 `app/build.gradle`、`settings.gradle`、应用 ProGuard 配置，或修改 Runtime ZIP。
否则一个扩展就会接管全应用的 Material、资源生成、签名和 AAB 配置。

VS 项目只在 RunOnly 构建后调用发布入口。Release 供 Windows 编辑器使用，Debug 不自动发布 Android。
考虑 VS 的快速“已是最新”检查，确保只修改 Java／共享头／依赖时，RunOnly 仍会进入发布流程。
命令和路径正确引用，检查退出码；VS 下不 pause，独立 bat 失败可 pause。保留 Python／子进程日志。

## 6. 验证与交付

1. 对核心结果使用独立参考或原实现对照。FTW 使用 Dijkstra 对照寻路代价，而不只断言程序没崩。
2. 检查 JNI 封送、Unicode、释放、异常、回调重入及 ACE 编号和参数。
3. 用完整开发 SDK 目录执行打包，检查没有 SDK 源码混入。缺库、坏哈希或编译失败不应产生新的发布 ZIP。
4. 检查实际 ZIP 以及最终 APK 中的 ABI 库、运行依赖和必要资源；不要只检查工作目录。
5. 用真实 Fusion Runtime 的默认模板构建 APK，不附加隐藏的本机依赖参数才宣称默认流程可用。
6. 设备验证对象／碰撞／热点／边框／滚动／图层；fixture 只证明调用边界，不能代替绘制验收。
7. 汇报具体工具版本、已执行检查和未覆盖部分。作者报告设备测试通过时，记录为作者反馈，
   不改写为自动化覆盖全部设备／功能。

FTW 的具体构建命令和参数以 `Extensions/Android_Gradle/FindTheWay-JNI.md` 为准。

## 参考

- [DarkEDIF 构建配置](https://github.com/SortaCore/MMF2Exts/blob/master/DarkEdif/Lib/FusionSDK.props)：开发阶段编译、构建后打包原生库。
- [DarkEDIF Bluewing Java 包装类](https://github.com/SortaCore/MMF2Exts/blob/master/DarkEdif/Bluewing%20Client/CRunBluewing_Client.java)：`System.loadLibrary` 加载。
- [官方旧 Android SDK](https://github.com/ClickteamLLC/android)：历史 native 模板和打包说明。
  其中 Ant、旧 ABI 和 assets/mmf 布局有年代限制，不能直接替代现代 Runtime 的 jniLibs 布局。
- [Gradle Java 兼容表](https://docs.gradle.org/current/userguide/compatibility.html)：选择 JDK 时查实际 Gradle 版本。
