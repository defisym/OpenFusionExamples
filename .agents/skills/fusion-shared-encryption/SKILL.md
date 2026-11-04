---
name: fusion-shared-encryption
description: Reuse and publish the common encryption runtime when porting or updating Fusion plugins. Prevent private per-plugin crypto implementations and preserve the Mac build/push to Windows pull/VS packaging workflow. Covers shared contracts; platform procedures are separate references.
---

# Shared encryption for Fusion plugins

## Scope

Use when a plugin consumes encryption, key derivation or encrypted assets, or when
changing the shared mobile encryption library/build/distribution. This is a shared
runtime contract, not permission to alter unrelated plugins or merge a PR.
Apply [common porting rules](../fusion-extension-porting/SKILL.md), including
compiled-component assessment and text/binary digest policy. Read the relevant
[Android](../fusion-android-porting/SKILL.md) or
[iOS](../fusion-ios-porting/SKILL.md) workflow before platform changes.

The shared encryption runtime is an explicit author requirement for this project.
It does not imply that every utility header, template or plugin algorithm should
be moved into a DLL. Keep small/stateless helpers modular; assess actual emitted
code, state ownership and exporter deployment before introducing another boundary.

## Required architecture

- Reuse `Extensions/_DeLib/Encryption`; preserve existing cipher, key derivation,
  Unicode and asset formats. Do not write a Java, Objective-C or plugin-local cipher.
- **Shared means shared by plugins at runtime**, not merely common source copied
  or statically linked into every plugin. Clients include `Encryption/CAPI.h`
  and call the stable `Encryption_*` C ABI. Keep algorithms, KeyGen and OpenSSL
  inside the single common runtime. Its implementation stays in the original
  `Encryption` sources; Android build integration lives under
  `Android_Gradle/app/src/main/cpp/Encryption` and iOS build integration under
  `iOS/Encryption`. Do not add a Mobile subtree to the Windows library.
- Do not put libcrypto, a private encryption implementation, or a renamed copy of
  the common library in each plugin package. Interface headers are permitted.
  Publish/build/cache the common runtime independently and declare client dependencies.
- Keep OpenSSL/C++ implementation symbols private; export only the supported C ABI.
  Validate ABI/version, target architectures, library identity and compatible STL.
  Never silently overwrite a conflicting common runtime already installed by another plugin.
- Inspect current shared modules and the target plugin's real publisher/VS hooks
  before adding another tool path. Reference existing publishing scripts (such as
  FTW) for discovery, incremental builds, errors, copy/install and archive behavior;
  do not invent a second manual delivery path or treat FTW-specific constants as universal.

## Cross-machine handoff

When Apple compilation belongs to the author, generate the real Xcode project,
shared schemes and commands even if the agent has no Xcode. The Mac build stages
owned plugin and common-library artifacts separately with SHA256/source manifests.
The author compiles and pushes those files; Windows git pull receives them, and
VS RunOnly invokes the publisher to validate, generate and update distribution
packages. Windows must not require Xcode or rebuild Apple code. Do not push
unbuilt placeholders or claim Apple/device validation from project-generation tests.

Preserve signing, runtime loader and exporter constraints. A generated ZIP is not proof of exporter integration. For the supported legacy
format, reuse the independent `.ext` codec and validate the decoded payload and
six project-insertion blocks. Distinguish a verified package from a successful
export, final link or device test.
Do not redistribute licensed exporter headers, Runtime sources or test projects.

## Validation and review

Check the client binary's actual dynamic references and absence of defined crypto
symbols, shared runtime exports, all supported slices, separate package ownership,
hashes/stale-source rejection and one-time dependency installation. Include failure
checks for missing, corrupt or conflicting artifacts. Run the actual publishing
entry point. Distinguish host fixtures, target builds, APK/exporter builds and devices.
Keep shared source changes reviewable and open/update the requested PR; do not merge.

- Android procedure: [references/android.md](references/android.md)
- iOS/Mac-to-Windows procedure: [references/ios.md](references/ios.md)
