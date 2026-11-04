# Build Fini in Xcode

This is an owned, standalone Xcode project. Generation does not require Xcode.
It reuses the existing C++ INI/encryption/codecs through CRunFini.mm and contains
no licensed exporter Runtime sources or Windows crypto binaries.

1. Keep this directory in its repository location: `Extensions/Fini/iOS`.
2. On your Mac, configure your licensed Fusion export and target OpenSSL:

   ```sh
   python3 Extensions/Fini/iOS/generate_project.py \
     --runtime "/path/to/Fusion-export"
   open Extensions/Fini/iOS/Fini.xcodeproj
   ```

   The export must contain `Classes/Extensions/CRunExtension.h`. OpenSSL prefixes
   must be `iphoneos-arm64`, `iphonesimulator-arm64`, `iphonesimulator-x86_64`,
   each containing `include/openssl/evp.h` and `lib/libcrypto.a`. If omitted, the
   common runtime builds these prefixes from pinned source. Supply iOS-built
   OpenSSL 3, not a macOS/Homebrew or Windows library. The shared runtime
   dependency builder can download and verify the pinned source; SDK sources are
   always supplied locally.
   Paths are saved in ignored `Config/Local.xcconfig`. You can edit that file
   directly instead. No signing identity is needed to build the library.
3. Select scheme **FiniXCFramework**, then Product > Build. It compiles all three
   slices, combines the simulator architectures and produces
   `work/fini-ios/Fini.xcframework` containing only the adapter. The common module
   independently produces `work/mobile-encryption/ios/OFEEncryption.xcframework`,
   a dynamic framework containing the shared encryption implementation.
   Output and errors are recorded in `work/fini-ios/publisher.log`.
4. Add both XCFrameworks to your exported app. Embed & Sign **OFEEncryption once**
   for all dependent plugins; do not embed copies per plugin. Link Foundation, libc++ and
   libz. Use `-ObjC` to retain the CRunFini class. The supplied
   `Config/FiniConsumer.xcconfig` records the consumer flags; merge them with the
   app's existing settings. Select your signing team in the app project when
   running a device build.

The **Fini** scheme is an ordinary static-library target useful for compiling
and debugging just the adapter for the selected device/simulator architecture.
Its `libFini.a` does not include encryption or libcrypto; **FiniXCFramework**
builds the client library and the separate common runtime. The project targets iOS 17+ for the reused C++20
library facilities. It is intended for Xcode 15+ with the corresponding SDK.

Ensure the exported app's extension loader instantiates CRunFini for extension
name Fini. A dynamic loader can use NSClassFromString; a loader with an explicit
class table needs the Fini import/creation entry. The repository's local SDK
CExtLoad.m contains that developer entry. If the app was exported without Fini,
replace Application.cci with your Fini test application's export before testing;
building a library alone does not add Fini events to an unrelated application.
After exporter/device checks, create the final Fusion `.ext` using the licensed
SDK's iOSExtMaker procedure. No `.ext` or successful Apple build is claimed here.

The generated project and shared schemes have been checked on Linux for stable
IDs, valid object references, correct source/configuration paths and scheme
references. Actual Xcode compilation and runtime validation must run on your Mac.
Regenerate the checked-in project after changing generate_project.py:

```sh
python3 Extensions/Fini/iOS/generate_project.py
python3 Extensions/Fini/iOS/tests/test_project.py
```

## Mac build/push -> Windows pull/VS packaging

After a successful FiniXCFramework build, the scripts stage these owned artifacts:

- `Extensions/Fini/iOS/Prebuilt/Fini.xcframework.zip` and `manifest.json`.
- `Extensions/_DeLib/Encryption/Mobile/Prebuilt/iOS/OFEEncryption.xcframework.zip`
  and `manifest.json`, shared once by all dependent plugins.

The author commits/pushes those four files after building on Mac. No script pushes
Git automatically. On Windows, git pull followed by VS Run_Only Unicode invokes
the existing Fini.bat/Fini.py entry point. It validates SHA256, required slices and
source freshness, then updates Fini-iOS.zip and the separate OFEEncryption-iOS.zip,
installs them when FusionRoot/the normal Fusion directory exists, and rebuilds
Fini_Release.zip. Windows does not need Xcode. To run just this handoff step:

```sh
python Extensions/_Script/Fini.py --ios-only
```

No Mac binaries are included until the author builds them. Before that first
build the publisher reports the missing handoff and leaves Apple outputs alone;
an incomplete, corrupt or source-stale committed pair fails instead of replacing
a known-good package. The handoff creates native XCFramework distribution ZIPs;
these are not mislabeled as exporter-tested .ext files. Final .ext/exported-app
integration still follows the licensed exporter's format and framework setup.
