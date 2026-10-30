# 排查顺序与已遇问题

先阅读日志中最后一次构建的错误。发布日志和 Gradle daemon 日志可能追加多次运行记录，旧错误
仍在文件开头不代表本次又失败。分别确认“发布器构建成功”和“Fusion APK 构建成功”。

## 从哪一层失败开始

| 现象／证据 | 优先检查 | 处理 |
| --- | --- | --- |
| VS 报 MSB3073，bat 返回非零 | 发布器日志末尾及子进程输出 | 修复具体依赖／原生构建错误，不能只忽略退出码 |
| VS 成功，Fusion 没有 APK | 实际安装的扩展 ZIP、Fusion JDK／SDK、Gradle 日志 | 检查发布目标是否与正在运行的 Fusion 安装目录一致 |
| 空白工程也不能打包 | Fusion 全局工具链／Runtime | 先排查共同问题，不将失败全部归因于 FTW |
| `Unsupported class file major version 69` | 日志里的 Java home、Java version、Gradle version | 此次为 JBR 25 + Gradle 8.14.3 不兼容，切换独立 JDK/JBR 21 后通过 |
| `checkDebugAarMetadata` 要求更高 API | 当前 compileSdk 与实际 AndroidX 依赖 | 调整 Fusion 的 Build API；本次 core 1.17 要求 API 36，不能只建议 API 35 |
| `Theme.Material3.DayNight.NoActionBar` 找不到 | Material 依赖是否被旧的扩展 Gradle 覆盖掉 | 移除默认模板覆盖，让 Fusion 默认依赖生效 |
| 大量 Runtime Java 签名／字段冲突 | 扩展 ZIP 是否混入开发 SDK | 限制自有 Java 收集边界；不要逐个给旧 SDK 打补丁或强转类型 |
| SDK 缺 CMake 且没有 sdkmanager | 已安装的 VS／其他 SDK／PATH CMake 和 Ninja | 复用兼容工具；确实缺失时再准备 Command-line Tools 和包 |
| iconv 校验在 Windows 头文件上失败 | 文件缺失、实际哈希、LF/CRLF | 区分缺文件和坏内容；仅允许头文件换行归一，二进制严格校验 |
| 设备报 `UnsatisfiedLinkError` | APK 实际 ABI、库名、依赖及符号 | 检查 jniLibs、`System.loadLibrary`、libc++、JNI 名称和 R8 规则 |
| 只在有热点的图片上偏半格 | 锚点、热点、像素左上角和各绘制路径 | 计算最终像素坐标，不用 DEBUG 开关临时掩盖 |
| 主体对齐但障碍旁边框偏移 | 三帧图片是否共享热点／参考原点 | 按目标帧热点与参考帧热点的差值调整，不要求原始锚点一致 |

## 日志位置与使用

- 发布器：`Extensions/Android_Gradle/FindTheWay-package.log`。看最终异常和原生编译／打包结果，
  留意创建 ZIP 不等于成功复制到用户正在使用的 Fusion。
- Gradle daemon：默认 `%USERPROFILE%/.gradle/daemon/<版本>/daemon-<pid>.out.log`；若设置
  `GRADLE_USER_HOME`，则在该目录下。按本次构建时间找对应记录，日志也可能含过往失败。
- 从错误中读取 Fusion 生成的临时工程路径，不假设每个版本都保留它，或都有某个导出／保留开关。
- 找不到有效构建日志时，核实是否已启动 Java/Gradle；不要反复修改算法来猜测启动阶段的问题。

IDEA 更新可能替换其内置 JBR 的主版本，路径不变而兼容性已变。给 Fusion 使用独立目录中的
JDK/JBR 21 可以隔离 IDE 自动更新；实际兼容关系以当前 Gradle 官方表和 Runtime 为准。
不要把构建 Java 所用 JDK、compileSdk、targetSdk、minSdk 和 NDK 原生目标 API 混为一个版本。

## 半格偏移案例：检查最终像素位置

设格子中心为 `C`，左上主体帧热点为 `H0`，边框帧热点为 `Hi`，业务偏移为 `D`。
Windows 此处使用共同图片原点，期望图片左上角为：

```text
P = C - H0 + D
```

Android 当前背景绘制路径会应用每帧自己的热点，左上角为 `P = A - Hi`。
因此应传入：

```text
A = C + Hi - H0 + D
```

主体 `Hi == H0`，直接传入 `C`。边框热点不同则必须带上差值。
32 像素格子中，若主体热点 `(16,16)`、边框热点 `(0,0)`，直接给边框传入同一个格子中心，
会相对主体偏移 `(16,16)`。先给所有帧统一加热点也不对。

专项检查至少覆盖：

- 全零热点；所有帧热点相同且居中；主体居中而边框为零；各帧不同且 X/Y 不对称。
- 可通行格旁的内部障碍和地图外边界，确认右／下业务偏移何时应用。
- 窗口滚动、图层偏移／可见性、即时粘贴与后续重绘，不把某一条绘制路径视为全部。
- 原始锚点、最终图片左上角、刷新矩形和碰撞采样分别检查。

本次上一轮测试只检查原始锚点一致，错误地接受了边框错位；修正后测试比较 Runtime 应用
各帧热点后的左上角。设备截图和 Windows 对照图用于校验这个预期，而非修改原本正确的 C++ 坐标。
