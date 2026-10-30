# GeneralDefinition portability and review notes

Platform differences are implemented in the existing shared helpers, which remain header-only.
`FindTheWay.h` and `AOE.h` use `HASHER_MOVE`, `Range`, `InsertSortedUpperBound`, and `_itos` again.
Other extensions can reuse the same implementations without expanding or replacing helpers at
individual call sites.

## Encoding policy

Windows retains the existing `MultiByteToWideChar` / `WideCharToMultiByte` implementations and
flags. `CP_ACP` continues to mean the Windows system ANSI code page. Non-Windows uses iconv.

There is no equivalent universal system ACP on non-Windows platforms. The compatibility default is:

```cpp
#ifndef DELIB_ACP_ENCODING
#define DELIB_ACP_ENCODING "UTF-8"
#endif
```

Define `DELIB_ACP_ENCODING` before the first helper include to override it, for example with
`"CP936"`. All translation units linked into the same executable/shared library must use the same
configuration. This is not OS encoding detection and does not change the process locale.
`CP_UTF8` always means UTF-8, independently of the ACP compatibility setting.

```sh
cmake ... -DDELIB_ACP_ENCODING=CP936
```

Mappings include UTF-8/7, UTF-16LE/BE, UTF-32LE/BE, ASCII, GB18030, and ISO-8859 variants. Other
numeric pages use iconv's `CP<number>` aliases. Available encodings depend on the iconv
implementation; unknown/unsupported pages return false. This does not promise every Windows page.

Conversion handles `WCHAR_T`, explicit input lengths, embedded NUL, `E2BIG` buffer growth, invalid
or incomplete sequences, and stateful-encoding flushes. Boolean overloads return false for encoding
errors or irreversible replacement and preserve the original output. Empty input retains the
Windows boolean interface's false result. String convenience overloads still ignore the boolean
result and return an empty string on failure. Both iconv input-pointer signatures are supported.
Conversion resources are closed on every exit path, including C++ allocation exceptions.

## Build dependencies

```cmake
include(path/to/Android_Gradle/cmake/DeLibHelpers.cmake)
target_link_libraries(your_target PRIVATE DeLib::Helpers)
```

CMake configuration and helper tests live under `Extensions/Android_Gradle/cmake` and
`Extensions/Android_Gradle/tests/helpers`, reflecting the current JNI work. `_DeLib` contains the
shared headers and their documentation.

The interface target supplies `_DeLib` and `_3rdLib` include paths and requires C++20. Shared
headers can use `#include "Base64.h"` without embedding relative directory layouts or selecting
a separate algorithm implementation.

Windows neither finds nor links iconv. Non-Windows uses CMake `FindIconv`; glibc commonly provides
it in libc, while other platforms may need a separate libiconv. Android has no public NDK iconv API:
supply headers and libraries compiled for the target ABI. Configuration fails explicitly when
the dependency is missing; it does not substitute a no-op conversion or host Linux library.

The current Android publisher includes validated iconv dependencies for its supported ABIs and
prebuilds JNI libraries. Those Android builds have passed. macOS/iOS toolchains remain separate
validation work; Linux or Android success does not establish their compatibility.

## Preserved interfaces and corrections

- Preserve `HASHER_MOVE` and its hash constants rather than duplicating their formulas in callers.
- Preserve `Range`, `UpdateRange`, and the four sorted-insertion helpers. AOE explicitly uses
  `Range<size_t>(dir, 0, 3)` to resolve argument-deduction conflicts on 64-bit platforms.
- Replace LPWSTR/LPSTR/LPCWSTR parameters with equivalent fundamental pointer types without
  introducing Windows aliases into other platforms.
- Implement `NewStr` with standard wide-character copying instead of `rsize_t/wcscpy_s`; copy
  before freeing the old buffer to support self-substring assignment and allocation-failure safety.
- Correct empty/all-space trimming bounds and the single-character `ToUpper` overload call.
- Pass unsigned-char values to narrow ctype operations; use towlower/towupper for wide characters.
  Comparisons follow the current C locale, do not promise Unicode case folding, and do not alter
  locale. Windows `StrIEqu` retains its CRT implementation.
- Preserve Windows path macro behavior. Non-Windows `CUR_FOLDER` / `RELATIVE_PATH` use
  `std::filesystem` parent paths, joining, and lexical normalization. Each evaluation returns an
  owning `PathString`, including evaluation of dynamic suffixes; no static/thread-local path
  buffers are used. Named objects provide `operator char*` / `operator const char*`; temporary
  pointer conversions are deleted to prevent dangling pointers after the expression.
- `CUR_FOLDER` retains its trailing separator. `RELATIVE_PATH` accepts absolute paths and lexical
  `..` normalization without requiring the target to exist. The result is not a preprocessor string literal.

```cpp
auto path = RELATIVE_PATH("asset.txt");
const char* readOnly = path;
char* writable = path;
// Keep path alive during C API use. Writes must stay within size()+1 bytes and retain the final NUL.
// const char* dangling = RELATIVE_PATH("asset.txt"); // Rejected: pointer conversion from a temporary.
```

Variadic calls do not apply implicit conversions: explicitly convert a path argument to
`const char*` when passing it to printf-style APIs.

Separate path objects, including copies, own independent buffers and can be used independently
across threads. Concurrent access involving a write to the same object still needs synchronization.
Do not retain pointers after the owner is destroyed, moved, or reassigned. Removing shared static
storage establishes result ownership; initialization of the old read-only function-local static
strings was itself already covered by C++11 thread-safe initialization.

`_DEBUG` and `NDEBUG` are not guaranteed to be mutually exclusive. Cache configuration gives
`_DEBUG` precedence, enables the default with only `NDEBUG`, and disables it when neither is
defined. All four combinations are compiled and exercised.

The trimming, case-conversion, and `NewStr` corrections also affect Windows callers and belong
in shared-code review. This work does not attempt to redesign every unrelated helper.

## Include audit

The following GCC 14 single-header probes recorded missing declarations when the include was
removed. They describe the tested toolchain rather than a guarantee about transitive includes:

| Include | Observed dependency |
| --- | --- |
| GeneralDefinition: cfloat | FLT/DBL/LDBL constants |
| GeneralDefinition, non-Windows: filesystem | Path macros; this GCC header also provides limits/cwctype declarations |
| GeneralDefinition, Windows: limits/cwctype | Existing declaration dependencies without filesystem |
| Non-Windows: cerrno, iconv.h | errno/E2BIG and iconv APIs |
| StringTraits: cstring | strlen |
| Base64: stdexcept | std::invalid_argument |
| Base64: climits | INT_MAX in strict length checks |
| FindTheWay, Windows: memory | make_unique; the tested non-Windows toolchain also obtains it through filesystem |

Unnecessary Base64 cstring/cctype, StringTraits cwchar, duplicate FindTheWay standard headers,
and the FINDTHEWAY_STANDALONE include branch were removed. StrNum explicitly includes the
GeneralDefinition helpers it uses. Add minimal direct dependencies when another toolchain
demonstrates a missing declaration.

## Verification

With the required Linux compiler, JDK, and CMake available:

```sh
python3 Extensions/Android_Gradle/tests/helpers/run_helper_tests.py --build-dir /tmp/delib-tests --sanitize
python3 Extensions/Android_Gradle/tests/run_host_tests.py --build-dir /tmp/ftw-tests --sanitize
```

Helper checks cover multiple translation units, UTF-8 and legacy-code-page round trips, ACP
overrides, malformed input, embedded NUL, long strings, stateful flushes, both iconv signatures,
public string/numeric/sorted-insertion helpers, and all four macro combinations. Eight threads
create 1,600 distinct paths from the same macro expansion site and check writable-copy isolation.
Compile-time checks reject unsafe writable conversions from temporaries and const objects.
ASan/UBSan checks run separately. The recorded ptrace-limited environment disabled LSan, so these
results do not claim a successful leak check.
