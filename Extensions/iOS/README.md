# iOS extensions

Each plugin owns a build project under this folder. FindTheWay contains its
CMake/CLI entry points, Core and Runtime adapter. Shared Runtime helpers live
in Helpers; publishing/build regression tests live in tests.

See [FindTheWay/README.md](FindTheWay/README.md) for build and artifact-transfer
commands. Build on Apple Silicon using CMake and Apple command-line tools;
device arm64 and simulator arm64 are separate platform slices. Keep private
Fusion SDK/exporter projects outside this repository.
