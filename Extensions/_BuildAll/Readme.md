# BuildAll

## Description

this script will search its parent folder, if any folder there contains sln with the same name of folder, script will
build it.

## Params

| Command    | Info                                                             |
| ---------- | ---------------------------------------------------------------- |
| --configs  | config to build, default is `Release Unicode` `Run_Only Unicode` |
| --target   | target to build, default is `rebuild`                            |
| --platform | platform to build, default is `Win32`                            |

e.g., you only want to run the post build event, you can use:

```powershell
Python ./main.py --configs "Release Unicode" "Debug" --target PostBuildEvent
```

## References

- <https://learn.microsoft.com/zh-cn/cpp/build/walkthrough-using-msbuild-to-create-a-visual-cpp-project?view=msvc-170>
- <https://learn.microsoft.com/zh-cn/visualstudio/msbuild/msbuild-command-line-reference?view=vs-2022>
- <https://learn.microsoft.com/zh-cn/visualstudio/msbuild/msbuild-targets?view=vs-2022>
