# iOS extensions

Each plugin owns a build project under this folder. FindTheWay contains its
CMake/CLI entry points, Core and Runtime adapter. Shared Runtime helpers live
in Helpers; publishing/build regression tests live in tests.

See [FindTheWay/README.md](FindTheWay/README.md) for build and artifact-transfer
commands. Build on Apple Silicon using CMake and Apple command-line tools;
device arm64 and simulator arm64 are separate platform slices. Keep private
Fusion SDK/exporter projects outside this repository.

## FindTheWay export configuration tool

After each Fusion iOS export, close Xcode and run the
[Python configurator](FindTheWay/configure_export.py) before opening the project.
From the repository root:

```sh
python3 Extensions/iOS/FindTheWay/configure_export.py /path/to/FindTheWay.xcodeproj
```

The Windows publisher also includes it in FindTheWay.ext as
`configure_findtheway_export.py`. When present in the exported folder, run:

```sh
python3 configure_findtheway_export.py .
```

It fixes arm64 settings, renamed Scheme references, required static linkage and
dependencies such as Box2D, and device/simulator FTW core selection. Use
`--dry-run` to preview; repeating the command does not duplicate configuration.
Both exported FTW archives must be present. It preserves higher minimum OS
versions and device signing; it does not rebuild the core or install advertising
SDKs. Reopen Xcode, select the App scheme, clean the build folder and build/run.

This tool is specific to FindTheWay's exported native libraries. See the
[FindTheWay instructions](FindTheWay/README.md#configure-the-fusion-export-on-mac)
for options and the complete Mac/Windows publishing workflow; other plugins
should derive their configuration from their own artifacts and targets.
