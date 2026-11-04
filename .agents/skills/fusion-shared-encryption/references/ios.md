# iOS shared encryption and Mac-to-Windows delivery

Use Objective-C++ to call the existing C++ plugin core and Encryption C ABI; JNI is
Android-only. Fini's standalone project is an example, not a dependency for other ports.
Build a client archive/XCFramework without libcrypto. Build one dynamic
OFEEncryption.framework/XCFramework through `iOS/Encryption/build.py` for all plugins, with a
stable install name and C export list. Embed/sign the common framework once in the app.
Keep its native handoff inputs in `iOS/Encryption/Prebuilt` and build tooling in
the iOS platform project; compile the original Encryption sources directly.

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

Preserve the verified PR #100 .ext flow: Windows consumes native artifacts and
packages the current loader/adapter using the independent shared format codec.
Verify decoded payload/CRC and PBX insertion blocks. A native XCFramework ZIP
alone is not an installable .ext, and a valid .ext is not proof of final linkage.
Use an idempotent target-aware configurator for missing exported project references;
preserve device signing, unrelated extensions/flags and a higher deployment floor.
Validate generated project syntax/references and the Mac/Windows manifest handoff
without calling those checks an Apple build. The author supplies Apple/device results.
