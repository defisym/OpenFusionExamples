# iOS shared encryption and Mac-to-Windows delivery

Use Objective-C++ to call the existing C++ plugin core and Encryption C ABI; JNI is
Android-only. Fini's standalone project is an example, not a dependency for other ports.
Build a client archive/XCFramework without libcrypto. Build one dynamic
OFEEncryption.framework/XCFramework from Encryption/Mobile for all plugins, with a
stable install name and C export list. Embed/sign the common framework once in the app.

Generate an actual .xcodeproj and shared schemes on the agent host. Configure local
licensed Runtime header paths through an ignored local xcconfig. Keep public SDK
locations portable and supply device/simulator build targets. A missing Xcode prevents
Apple compilation, not generation of the requested project and delivery scripts.

Honor this repository's author handoff: Mac builds -> separately stages owned client
and shared artifacts with manifests -> author commits/pushes -> Windows git pull ->
VS RunOnly's existing publisher validates/stages those artifacts and creates/updates
release packages. Preserve the single publishing entry point and propagate errors.
Do not ask the author to manually reconstruct packages after every Mac build.

Do not run git push from the Mac build script, commit machine-local configs or include
licensed SDK source. The author controls publishing compiled Apple files. Windows can
skip Apple packaging while no Mac build is available, but must reject incomplete,
corrupt or source-stale committed artifacts without replacing a known-good package.

Record the actual output format. A native XCFramework handoff package is not an
exporter-tested .ext; use the licensed exporter packaging mechanism where required.
Validate generated project syntax/references and the Mac/Windows manifest handoff
without calling those checks an Apple build. The author supplies Apple/device results.
