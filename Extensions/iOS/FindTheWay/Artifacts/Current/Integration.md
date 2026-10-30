# FindTheWay iOS integration

Build host: Apple Silicon. Slices: iOS arm64 and iOS Simulator arm64.
Minimum iOS selected for this build: 17.0.

Add the static FindTheWayCore.xcframework to the exported App target
with Do Not Embed. Link Apple libc++ and iconv. The Runtime adapter
includes FindTheWayCore.h; it does not compile the shared C++20 core.

This build command verifies source/file hashes and packaging metadata.
It does not establish full Runtime ACE/export/device validation.
The current adapter is incomplete; a native bundle is not a usable .ext.
