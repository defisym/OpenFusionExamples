---
name: fusion-extension-porting
description: Apply common rules when porting Clickteam Fusion extensions across Windows, Android, iOS, or HTML5, especially shared behavior, common native component boundaries, cross-platform artifact transfer, manifests, and publishing verification. Use platform skills for SDK and build procedures.
---

# Common Fusion extension porting rules

## Scope

Apply these rules across extension ports. Keep platform SDK, Runtime and build
procedures separate: use [Android porting](../fusion-android-porting/SKILL.md)
or [iOS porting](../fusion-ios-porting/SKILL.md) for the relevant target.
Derive paths and commands from the target project; do not require another plugin.
Write skills and repository documentation in English.

Use the current Windows C++ implementation as the behavioral reference for
portable features. Reuse shared algorithms; adapt platform-specific object,
resource and Runtime access through the actual target SDK. Keep shared C++
changes reviewable and honor the author's review requirements.

## Evaluate common native components

Assess this when starting another plugin port or introducing a shared dependency;
do not automatically split every plugin into multiple dynamic libraries.

- Establish the actual linkage from build targets and binaries. An XCFramework
  is a container and may hold static libraries; do not infer dynamic linkage
  from its name or from another platform's implementation.
- Inventory common headers and compiled dependencies. Macros, constants,
  templates and inline helpers do not imply that an entire utility header or
  library is included in the final binary. Inspect emitted symbols, link maps
  and final sizes to identify actual duplication and unused dependencies.
- Audit duplicate symbols, visibility, ODR/configuration differences, global
  state, caches and ownership across plugins. Header-only code is not necessarily
  stateless or collision-free. Linker folding or dead stripping is not a general
  guarantee of deduplication or isolation across independent libraries.
- Prefer focused includes and source/build modularity for small, stateless
  helpers. Interface, static or object-library targets can express shared
  dependencies without adding a runtime deployment contract. This alone does
  not remove duplication across separately linked plugin binaries.
- Consider a common dynamic library when several plugins demonstrably duplicate
  substantial compiled code, or require one deliberately shared service/state.
  Balance that benefit against version coupling, independent plugin updates,
  loading, packaging and deployment complexity. Define which module owns state
  and how conflicting dependency versions coexist before adopting the design.
- If a runtime boundary is justified, prefer a versioned C ABI with opaque
  handles, explicit lengths and ownership. Keep allocation/release in the same
  owning module; avoid exposing STL objects, C++ class layouts or exceptions.
  Moving templates into a DLL does not by itself provide a stable C++ ABI.
- Check the target SDK/exporter's dependency support, architectures, loader
  paths, embedding/signing and name-collision behavior using its platform skill.
  Preserve the intended automated build/package/export workflow. A host-side
  library build is insufficient evidence that Fusion will deploy it correctly.

State the assessment and evidence briefly. Keep the current design when there
is no concrete benefit; propose shared C++ changes separately for author review.

## Git checkout, artifact text and hashes

Git's Windows `core.autocrlf` can convert LF to CRLF in generated artifact text,
not only source files. Public headers inside a prebuilt framework, XML metadata
such as Info.plist, and integration Markdown can change while the compiled
library remains byte-identical. A manifest hash mismatch alone is not evidence
that the native library is stale or needs rebuilding.

- Define a consistent digest policy in the producer and consumer. Normalize
  CRLF to LF for explicitly identified text artifacts as well as source
  fingerprints. Check the format before treating metadata as text: a binary
  plist is not XML. Preserve encoding and all content except the agreed newline
  conversion; actual header or metadata edits must still fail verification.
- Keep binary hashes byte-exact, including static/shared libraries, executable
  extensions, images and package archives. Never apply newline replacement to
  every payload or skip verification to make a transfer pass.
- Compare device/platform copies of public headers using the same text policy.
  Mixed LF/CRLF checkouts must not appear to change the compiled ABI.
- Use .gitattributes when byte-preserving checkout is required, but account for
  existing working trees: adding an attribute does not necessarily rewrite
  files already checked out. A consumer fix must work with existing transfers.
- Report the exact missing, unexpected or mismatched paths. Distinguish source
  dependency changes, binary corruption, and text checkout differences before
  recommending another native build. Do not regenerate a manifest merely to
  accept unexplained differences. Preserve previous outputs on failure.

## Required transfer regressions

Verify LF and CRLF text, mixed header endings, rejection of real text edits, and
rejection of binary changes even when the only changed bytes are newline bytes.
Exercise the actual publisher with real transferred artifacts where available.
For tracked files, reproduce Windows Git conversion without changing the user's
checkout using `git -c core.autocrlf=true cat-file --filters HEAD:<path>`.
Confirm validation, package contents and installation after that conversion;
passing local LF-only tests is insufficient evidence of Windows compatibility.
Report packaging, exporter, final-link and device validation separately.
