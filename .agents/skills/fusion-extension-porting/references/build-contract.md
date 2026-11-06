# Shared build and publishing contract

Read the [baseline](../SKILL.md) before research or implementation. This is the
required behavior for new and revised porting workflows, not a claim that every
current publisher implements it. Extend existing tools and codecs first.

## Configuration and orchestration

Use one common entry point and a documented configuration format. Keep target
versions and commands in configuration/platform handlers rather than in copied
plugin scripts. Declare:

- Selected plugins and enabled platforms; explicit RunOnly configuration names.
- Plugin-to-common-component and component-to-component dependencies, including
  the existing source/build target, component version and ABI contract.
- Toolchain, architecture/ABI, SDK, deployment target, configuration and static
  or dynamic linkage. Keep device and simulator products distinct.
- Platform project/build/staging locations, artifact manifests, package ownership
  and optional install destinations. Never require another plugin's paths.
- Native build mode: local supported toolchain, configured remote Mac builder,
  or verified prebuilt handoff. Declare which mode a run actually used.

Resolve the complete dependency graph before changing published outputs. Reject
unknown components, missing configuration and cycles with a dependency path.
Build in topological order. Deduplicate by component **and build variant**:
version, source/configuration fingerprint, ABI, SDK/platform, deployment target,
toolchain and linkage. Building Encryption for device arm64 does not satisfy
simulator arm64; changing a dependency must invalidate affected consumers.

Use native build-system incremental checks plus verified manifests, not mere file
existence. If a selected plugin requires Encryption and it is missing or stale,
one invocation must build Encryption first, then the plugin. Several consumers
of the same Encryption variant share one build and one deployment decision.
A failure stops dependent builds and final publishing; never substitute an old
library, silently skip an enabled platform, or regenerate hashes to bless it.

## iOS intermediate project and batch script

Provide a reusable **batch build script** under `Extensions/iOS` together with
the intermediate library build project/generator. This is a native-library
project, separate from Fusion's exported application. The script accepts the
selected plugins/components and build variants, expands dependencies, generates
or updates the intermediate project, builds dependencies and clients, creates
XCFrameworks if used, and writes verified handoff manifests. It must support a
single plugin and multiple plugins without hand-building their dependencies.
Keep per-plugin configuration under its iOS directory and shared component
configuration in a sibling common-component directory.

For example, selecting a plugin with `Plugin -> Encryption -> crypto backend`
must produce/verify that entire chain. Only the Encryption component owns its
crypto linkage when the dynamic common-runtime design is selected. A second
plugin in the same invocation must not rebuild the same Encryption variant or
embed an incompatible second copy.

CMake and Xcode can both express these dependencies:

- In CMake, use real component targets and `target_link_libraries` for link
  dependencies. Use `add_dependencies` for ordering not implied by linkage, and
  declared outputs/byproducts and dependencies for generated files. Generate an
  Xcode project with CMake's Xcode generator when that fits the existing tools.
- In a maintained Xcode project, configure Target Dependencies and link products
  of the corresponding targets. For separate projects, add them to a workspace,
  establish cross-project target/product references and dependency relationships,
  and provide shared Schemes usable by `xcodebuild`. Merely placing projects in
  a workspace or listing framework search paths does not establish build order.
- A CMake IMPORTED target or a prebuilt framework file reference does not build
  its producer. Have the batch orchestrator build/verify that producer first,
  then configure consumers, or connect a real producer target explicitly.
- If independent per-platform projects cannot share a native graph, the batch
  orchestrator must still resolve and execute their dependency closure. Avoid
  recursive per-plugin scripts that rebuild the same common component.

Derive actual schemes, target names, configurations and commands from generated
projects. Use `xcodebuild` for unattended builds; do not require opening the GUI.
Validate shared Scheme references and source/product paths after generation or
renaming. Do not equate listing targets or generating a project with compiling it.

A self-contained core without external dependencies may be static. For dynamic
frameworks, verify Mach-O type, install names, runtime search paths, transitive
libraries, one-time embedding and signing in the final app. A static XCFramework
must be linked rather than embedded as a dynamic framework. Verify each device
and simulator slice independently and never mix platform slices via lipo.

## VS RunOnly and Mac handoff

```text
VS RunOnly / common publishing entry point
  -> load configuration and resolve selected platform/component dependencies
  -> Android: build common components and JNI clients for configured ABIs
  -> iOS: batch-generate/build on a configured Mac, or verify Mac handoff
  -> validate all selected artifacts and stage platform packages
  -> create final release archive and perform configured installation
```

Full Xcode and an Apple toolchain are required for iOS compilation. Windows can
invoke the configured Mac batch builder and retrieve its outputs, or verify and
package previously built artifacts. In prebuilt mode, missing/stale dependencies
must fail with the exact component/source paths and the batch command needed on
Mac. Do not claim Windows rebuilt iOS or silently switch modes. Native-build mode
must build missing dependencies automatically within the same invocation.

Integrate one ordered VS target with the existing release flow; do not retain
old per-platform hooks that publish twice or race with the final archive. Handle
VS fast up-to-date checks so changes to adapters, shared sources, configuration
and dependencies reach the publisher. Exclude design-time builds, preserve the
existing Release/Debug semantics, quote paths, log commands, propagate nonzero
exit codes and never pause for keyboard input under MSBuild.

Retain the verified independent .ext codec and platform publishers where useful;
factor their common orchestration rather than copying or rewriting them. Track
compiled adapter sources in native fingerprints. An edit to an adapter compiled
into a library requires rebuilding it; an export-only source adapter edit need
not rebuild an unchanged native core.

Stage into temporary locations and validate before replacing known-good packages.
Do not include stale products for a disabled platform in a new combined release.
Publish the final archive only after all enabled platforms succeed. For common
runtime deployment, reuse identical artifacts and reject name/version/hash
conflicts; define cleanup without deleting another plugin's required component.
Keep private SDK material and build caches out of packages and commits.

## Acceptance checks when implementing the workflow

- A clean selected-plugin build automatically builds all missing dependencies.
- Two plugins sharing a component build/deploy each identical variant once;
  differing variants remain separate and conflicting runtime versions fail.
- An unchanged repeat is incremental; core, dependency, toolchain and relevant
  configuration changes rebuild the affected products.
- Invalid graphs and failed dependencies stop before publishing. Existing
  packages survive failures; disabled platforms cannot leak stale output.
- VS RunOnly uses the same entry point and finishes packaging before archiving;
  Debug, Release and design-time builds do not accidentally trigger mobile work.
- The iOS batch script works for device and simulator separately. Test generated
  projects with actual `xcodebuild` on Mac, not only project-text assertions.
- Exercise both remote/native and prebuilt modes if provided; stale/missing
  handoffs fail explicitly. Apply the baseline's text/binary hash regressions.
- Inspect final APK/app dependency loading, common-library coexistence, and iOS
  linkage/embedding/signing. Report fixture, build, packaging and device evidence
  separately; unavailable platforms remain explicitly unverified.
