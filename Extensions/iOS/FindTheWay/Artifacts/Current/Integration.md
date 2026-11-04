# FindTheWay iOS integration

Build host: Apple Silicon. Slices: iOS arm64 and iOS Simulator arm64.
Minimum iOS selected for this build: 17.0.

Transfer Artifacts/Current to Windows. RunOnly verifies the core and
packages the current Runtime sources into FindTheWay.ext automatically.
The extension autolinks the correct device/simulator archive, libc++
and iconv using the supplied exporter SRCROOT library search path.
The Runtime sources require C++11; shared C++20 stays precompiled.

This command verifies source/file hashes and packaging metadata.
It does not establish Fusion export or final App/device validation.
