# GeneralDefinition 的跨平台改造（待审核）

本提交在现有公共 helper 中实现平台差异，继续保持 Header Only。
`FindTheWay.h`、`AOE.h` 恢复调用 `HASHER_MOVE`、`Range`、`InsertSortedUpperBound` 和 `_itos`。
其他扩展可以复用同一份实现，不必在调用处逐个展开或替换 helper。

## 编码策略

Windows 保留 `MultiByteToWideChar` / `WideCharToMultiByte` 的原实现及参数标志；
`CP_ACP` 继续代表 Windows 系统 ANSI 代码页。非 Windows 分支采用 iconv。

非 Windows 没有相同含义的系统 ACP，因此按项目兼容策略默认使用 UTF-8：

```cpp
#ifndef DELIB_ACP_ENCODING
#define DELIB_ACP_ENCODING "UTF-8"
#endif
```

在第一次包含 helper 之前定义 `DELIB_ACP_ENCODING`，可覆盖默认值，例如 `"CP936"`。
所有链接到同一可执行文件/动态库的翻译单元必须使用一致配置。它不是操作系统编码检测，
也不会改变进程 locale。`CP_UTF8` 仍明确表示 UTF-8，不随 ACP 兼容配置改变。

```sh
cmake ... -DDELIB_ACP_ENCODING=CP936
```

编码映射包含 UTF-8/7、UTF-16LE/BE、UTF-32LE/BE、ASCII、GB18030、ISO-8859 系列，其他编号
交给 iconv 的 `CP<number>` 别名解析。具体可用编码受 iconv 实现影响；未知/不支持的代码页
返回 false，不宣称覆盖 Windows 的全部代码页。

iconv 转换显式处理 WCHAR_T、输入长度、嵌入 NUL、E2BIG 扩容、非法/截断输入和状态编码刷新。
bool 接口遇到编码错误或不可逆替换返回 false，且保留原输出；空输入与原 Windows bool 接口
保持返回 false。返回字符串的便捷重载继续忽略 bool 结果、失败时返回空字符串。
两种 iconv 输入指针签名均可编译。资源在所有退出路径关闭，包括 C++ 分配异常。

## 构建依赖

```cmake
include(path/to/Android_Gradle/cmake/DeLibHelpers.cmake)
target_link_libraries(your_target PRIVATE DeLib::Helpers)
```

测试用例和 CMake 配置均位于 Android_Gradle：tests/helpers 与 cmake，仅用于当前 JNI 工作；
_DeLib 内仅保留共享头文件及其说明文档。

该接口目标统一提供 `_DeLib`、`_3rdLib` 搜索路径及 C++20 要求。恢复
`#include "Base64.h"`，无需在源码中写相对目录或切换独立版算法。

Windows 不查找或链接 iconv。非 Windows 使用 CMake `FindIconv`；Linux/glibc 通常内建于
libc，其他平台可能需要独立 libiconv。**Android NDK 没有公共 iconv API，必须提供针对目标
ABI 编译的 iconv 头文件和库**，例如通过 FindIconv 的 `Iconv_INCLUDE_DIR`、`Iconv_LIBRARY`
配置。没有依赖时配置明确失败，不提供静默空实现，也不会下载或绑定主机库供 Android 使用。
macOS/iOS/Android 工具链尚未实测；主机 Linux 验证不能替代这些平台的验收。

## 保留接口及必要修正

- `HASHER_MOVE` 及哈希常量完全保留，不在各调用点复制公式。
- `Range`、`UpdateRange`、四个排序插入 helper 保留；AOE 显式使用
  `Range<size_t>(dir, 0, 3)` 解决 64 位平台参数推导冲突。
- LPWSTR/LPSTR/LPCWSTR 参数改写为等价基础指针类型；不向其他平台注入 Windows 类型别名。
- `NewStr` 用标准宽字符复制替代 `rsize_t/wcscpy_s`，先复制再释放，支持自身子串赋值和分配失败保护。
- 修复空串/全空格裁剪越界，以及 `ToUpper` 的单字符重载调用错误。
- 窄字符 ctype 输入转 unsigned char；宽字符使用 towlower/towupper。比较遵循当前 C locale，
  不承诺完整 Unicode case folding，也不修改 locale。Windows StrIEqu 保留 CRT 实现。
- Windows 路径宏保持原行为；非 Windows 的 CUR_FOLDER/RELATIVE_PATH 使用 std::filesystem 的
  parent_path、路径拼接及 lexically_normal。每次求值按值返回拥有 std::string 的 PathString，
  不使用静态或 thread_local 路径缓冲区，动态后缀也逐次求值。具名对象提供 operator char* /
  operator const char*；临时对象的指针转换被删除，防止表达式结束后悬垂。
  CUR_FOLDER 保留尾部分隔符；RELATIVE_PATH 支持绝对路径及 .. 词法规范化，不要求目标存在。
  结果不是预处理器字符串字面量。

```cpp
auto path = RELATIVE_PATH("asset.txt");
const char* readOnly = path;
char* writable = path;
// 传给 C API 时保持 path 存活；写入限于 size()+1 字节内，并保留结尾 NUL。
// const char* dangling = RELATIVE_PATH("asset.txt"); // 编译失败：不能从临时对象取指针。
```

可变参数接口不会应用隐式转换，例如 printf 的路径参数应显式转换成 const char*。

不同路径对象（包括副本）拥有独立缓冲区，可以分别用于不同线程。同一对象并发写入或读写
仍需调用方同步；对象销毁、移动或赋值后，不应继续使用之前取得的指针。这次取消静态存储
是为了明确所有权；旧版本只读 static 字符串的初始化本身受 C++11 的线程安全规则保护。
- `_DEBUG` 与 `NDEBUG` 无语言级互斥保证。缓存开关明确以 `_DEBUG` 优先；仅 NDEBUG 时默认启用，
  两者均未定义时关闭。四种组合都编译和运行检查。

以上空串、大小写和 NewStr 修正也影响 Windows 的对应公共 helper，仍需作者审核及 Windows 回归。
未展开修改其他 helper 的既有设计问题，也未重写第二个提交中的序列化/路径算法修正。

## include 审计

按“移除后复现编译错误才保留”的要求，使用 GCC 14 单头探针核定：

| 新增头 | 复现的问题 |
| --- | --- |
| GeneralDefinition: cfloat | FLT/DBL/LDBL 常量未声明 |
| GeneralDefinition（非 Windows）: filesystem | 路径宏使用 std::filesystem；当前 GCC 的该头也提供 limits/cwctype 声明 |
| GeneralDefinition（Windows）: limits/cwctype | 保留上一提交的相应声明依赖；不包含 filesystem |
| 非 Windows: cerrno、iconv.h | errno/E2BIG 与 iconv API 的实现依赖 |
| StringTraits: cstring | strlen 未声明 |
| Base64: stdexcept | std::invalid_argument 未声明 |
| Base64: climits | 第二个提交严格长度检查使用 INT_MAX |
| FindTheWay（Windows）: memory | 保留 make_unique 的声明依赖；非 Windows 已由 filesystem 提供 |

已撤掉此前多加的 Base64 cstring/cctype、StringTraits cwchar，以及 FindTheWay 的重复标准头和
FINDTHEWAY_STANDALONE 包含分支。StrNum 显式包含实际依赖的 GeneralDefinition，保留数字 helper。
这些是当前工具链的编译证据；其他工具链若出现实际缺声明，再补最小依赖。

## 验证命令

Linux/GCC、JDK、CMake 环境下运行：

```sh
python3 Extensions/Android_Gradle/tests/helpers/run_helper_tests.py --build-dir /tmp/delib-tests --sanitize
python3 Extensions/Android_Gradle/tests/run_host_tests.py --build-dir /tmp/ftw-tests --sanitize
```

helper 测试包括多翻译单元链接、UTF-8 与旧代码页往返、ACP 覆盖配置、错误输入、嵌入 NUL、
长文本、状态编码刷新、两种 iconv 函数签名、所有公开字符串/数值/排序 helper 及四种宏组合。
另外以 8 个线程从同一宏展开位置创建 1600 个不同路径，检查副本可写隔离；编译期检查禁止
临时对象和 const 对象获取不安全的可写指针。ASan/UBSan 检查另行运行；当前环境受 ptrace 限制，脚本关闭 LSan，不宣称通过泄漏检查。
