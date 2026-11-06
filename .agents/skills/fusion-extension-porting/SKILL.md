---
name: fusion-extension-porting
description: Apply common rules when porting Clickteam Fusion extensions to any platform, including initial HTML5 and future-platform research. Read before proposing or implementing a port; enforce C++ reuse, platform layout, common components, dependency-aware builds and functional PR ordering. Use platform skills for SDK and build procedures.
---

# Common Fusion extension porting rules

## Scope

These are mandatory baseline rules for **all** platforms, including Windows,
Android, iOS, HTML5 and platforms whose implementation has not yet been decided.
They apply before feasibility research, prototypes, build experiments and production
ports. An exploratory task does not authorize a replacement core or a competing
build/publishing system. Read this skill first, then the applicable platform skill:
[Android](../fusion-android-porting/SKILL.md) or
[iOS](../fusion-ios-porting/SKILL.md). Platform procedures refine this baseline;
they do not relax it. Explicit current user instructions take precedence.

Before proposing or writing code:

1. Read repository instructions, this baseline, the target platform skill and its
   references, current build scripts/configuration, and relevant PR review notes.
2. Inventory existing C++ classes, common libraries, Runtime helpers, ABI adapters,
   publishers and tests. Identify what will be reused and the concrete platform
   API that requires each new implementation. Inspect actual code before inventing
   a substitute. Reuse existing interfaces and tools instead of implementing a
   parallel framework, helper library, codec or publisher.
3. Record the source-to-platform layout, shared dependency graph, build/package
   entry point and validation plan. Derive commands and versions from the target;
   another plugin's settings are examples, never repository-wide requirements.
4. If the platform has no skill yet, apply this baseline immediately. Document
   unresolved SDK/linkage/export questions as unknowns, then add platform-specific
   details when established. HTML5 must assess reuse of the C++ core (for example,
   a suitable WebAssembly toolchain); do not assume a JavaScript algorithm rewrite.

Write skills and repository documentation in English. The standing requirements
below do not need to be reconfirmed for each new plugin.

## C++ is the primary implementation

All platform-independent functionality must reuse the existing C++ core, not
merely reproduce equivalent behavior in Java, Objective-C, Swift or JavaScript.
Platform languages handle only platform-bound APIs, Runtime access and ABI
marshalling. Keep algorithms, data models, formats and portable policy in C++.

The original C++ implementation was written for Windows and **may be changed** to
support other platforms. Adapt original source files with narrow platform or
feature macros and standard facilities; preserve the default Windows behavior
and existing public interfaces unless an intentional change is separately reviewed.
Do not fork a mobile core, expand an existing helper at its call sites, or invent
new names to avoid repairing the original dependency. Add only required includes;
use configured include paths and existing focused utility headers. Platform macros
and debug macros are independent: test their relevant combinations rather than
assuming mutual exclusion. Keep portable fixes in the shared implementation.

## Repository layout

Treat `Extensions/Android_Gradle` and `Extensions/iOS` as platform project roots.
Mirror the **logical organization** of `Extensions`: plugin code belongs to the
platform's plugin area, shared libraries/helpers to its common library area,
and shared build utilities to its common tooling area. Preserve the platform's
required Java/Gradle or Xcode directory conventions.

- `Extensions/<Plugin>` remains the Windows project and established `ToInstall`
  release staging area. Do not create Android, iOS or Mobile development subtrees
  inside Windows plugin or library directories.
- Keep iOS adapters, intermediate library projects, tests, build/configuration
  scripts and handoff inputs under `Extensions/iOS`. Plugin-specific material
  belongs under `<Plugin>`; a common component such as Encryption has its own
  sibling area and is never owned by the first plugin that consumes it.
- Keep Android Java/JNI sources, CMake targets, dependency preparation, tests and
  docs under `Extensions/Android_Gradle`, using `app/src/main`, `cmake`, `tests`
  and existing common helper packages. Shared component targets are siblings of
  plugin targets, not private copies under each plugin.
- Original platform-independent C++ remains in its existing shared source files;
  platform targets reference those files. Truly platform-specific implementations
  belong in the corresponding platform root. Reusable cross-platform orchestration
  may live in `Extensions/_Script`; platform build logic stays with its platform.
- Apply the same ownership mapping when introducing a future platform root. Do
  not move the entire existing platform tree simply to make paths look identical.

For every move, update includes, source lists, project references, manifests,
publishers, tests and documentation. Existing SDK/export paths may differ from
repository paths; define their mapping rather than scattering development files.

## Reusable Runtime object helpers

Object lookup, selection and creation are platform infrastructure. Keep separate
ObjectBase, ObjectSelection and ObjectCreation classes, following the existing
Windows and Android organization. Platform-bound common helpers may have their own implementation, but preserve
Windows filename stems, class/interface names and portable method contracts
(language-specific suffixes such as .java or .mm are expected). Solve SDK name
collisions through directories, packages/namespaces and explicit includes, not
plugin prefixes or new parallel APIs. Existing divergent names are compatibility
debt: document their mapping and migrate callers together, retaining a thin alias
when necessary rather than silently breaking existing ports. Plugin-specific predicates, coordinates and ACE dispatch belong
in the consuming adapter. Compare the full portable API with current reference
implementations, including capabilities the first consumer does not yet use.

Test these helpers without a plugin's computational core. For native headers,
also compile/link multiple consumers to detect duplicate definitions; use one
consistent helper revision. Record concrete Runtime differences rather than
forcing Windows structure layouts or build procedures onto another platform.

## Review and handoff

Split PR commits by **functional logic**, with C++ core changes first and platform
changes last. Separate independently reviewable core problems (for example,
ownership, decoding and clocks); put the relevant focused tests with each fix.
Do not hide C++ behavior changes inside a final Java/JNI/Objective-C++ integration
commit. Distinguish portability fixes, intentional behavioral changes and a new
ABI around unchanged classes. Follow with shared component/build foundations,
then platform adapters, integration, packaging and their documentation. Avoid a
single catch-all core commit when it contains unrelated functional changes.

Preserve existing review checkpoints and unresolved feedback. Automated tests
are not author approval. Keep necessary implementation work moving under current
authorization; do not merge without authorization. Rewriting history is not a
routine prerequisite; when authorized, preserve other work, verify the resulting
file tree and use an expected-head lease for remote updates.

Exclude private SDK exports, build caches and temporary investigation output.
Retain native artifacts and verification metadata only when they are required
inputs to the agreed cross-platform handoff.

## Common components and linkage

Build common libraries as reusable common components. Plugins depend on those
components and their existing interfaces; they must not each implement a similar
library or maintain a private fork. Share source ownership, build definitions,
version/configuration and dependency preparation, even before a second consumer
is added. Header-only templates/inline utilities may remain shared headers; this
is not permission to duplicate their implementation under plugin directories.

Static libraries are allowed for self-contained cores with no external
dependencies. Dynamic linkage is not mandatory for every iOS core. Encryption is
a cross-plugin common module: prefer evaluating a single dynamic component
(Android .so / iOS dynamic framework) so consumers do not each link their own
crypto implementation. Reuse the existing Encryption C ABI and implementation.
Confirm actual runtime/exporter deployment support and document the decision;
do not silently fall back to per-plugin encryption copies if integration fails.

Inspect build targets, symbols and final dependencies. An XCFramework is a
container, not proof of dynamic linkage. Static/object targets can centralize
compilation but do not deduplicate code or state across independently linked
plugins. For common dynamic components define version/ABI compatibility, state
ownership, loader paths, embedding/signing and a single deployment owner. Reject
conflicting versions instead of allowing the last plugin package to overwrite
another component. Test at least two consumers together.

Prefer a versioned C ABI across native module boundaries, with opaque handles,
explicit lengths, exception translation and allocation/release in the owning
module. Do not expose STL objects or compiler-specific class layouts. Inspect
ODR, symbols, global state and configuration differences for shared headers too.

## Configuration-driven build and publishing

Every port must extend the common workflow, not invent another per-plugin build
system. Editing and generalizing the existing build scripts is part of porting.
VS RunOnly invokes one noninteractive entry point that reads enabled platforms
and dependencies, builds or verifies them, packages platform outputs, and only
then produces the final release archive. Other configurations retain their current
Windows behavior. Future platforms join through configuration and a platform
handler; they do not require copying the whole publisher.

Read [the build contract](references/build-contract.md) before implementing or
changing scripts, VS hooks, generated projects or platform dependency setup.
The contract defines dependency closure, the iOS batch script/intermediate
project, Mac handoff, failure handling and acceptance checks. Existing plugin
publishers are implementation examples, not evidence that this generalization
has already been completed repository-wide.

## Evidence and existing ports

Read [the reviewed sources and migration notes](references/reviewed-sources.md)
for the PR evidence behind these rules. Historical static FTW artifacts, helper
names and plugin-specific scripts describe existing behavior; they do not waive
this baseline. Do not claim an existing port has migrated merely because its
skill was updated. Distinguish implementation, host/fixture tests, platform
compile/link, exporter packaging and device validation.

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
