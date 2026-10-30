# iOS extension build projects

Each extension owns one directory under this folder, starting with FindTheWay.
The directory name identifies the target platform; Apple Silicon macOS is the
build host. All supported outputs are arm64: iOS device and iOS Simulator are
separate platform slices. No Intel or Rosetta build is required.

Use CMake plus Ninja (Make fallback) and Apple's command-line tools. The scripts
never open Xcode or require a manually maintained Xcode application project.
Full Xcode must be installed and selected because the standalone Command Line
Tools package does not supply both iOS SDKs. Each plugin keeps its core and Runtime adapter in its own directory. Platform
helpers live in Helpers and publishing regression tests live in tests.

The old bundled SDK Xcode project, Application.cci, sample template, IDE user
state and SDK tool archives were removed. Use a separately installed Fusion iOS
exporter project for Runtime reference and testing; do not vendor it here.

See FindTheWay/README.md for the current build/handoff commands and remaining
plugin implementation work. This workflow is independently implemented; DarkEdif
was used only as an architectural reference, with no code copied from it.
