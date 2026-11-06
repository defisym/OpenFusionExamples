# Reviewed sources and migration notes

Baseline updated from the author's instructions on 2026-11-06, the checked-out
master at `c48efa31`, and the following accessible PR descriptions/reviews.
Project chat history unavailable to the current session is not claimed as read.
Re-read current code and PR status when executing a port; open PR designs and
review checkpoints are evidence, not automatic approval or merged implementation.

| Source | Reusable requirement |
| --- | --- |
| [#94 Android JNI](https://github.com/defisym/OpenFusionExamples/pull/94) | Reuse C++ through JNI, retain existing helper interfaces, make narrow portability changes, verify debug/platform macro combinations, prebuild native libraries and preserve Fusion's default Gradle templates. Reviews request necessary includes only and changes in original helpers instead of expanding replacements at callers. |
| [#99 object helpers](https://github.com/defisym/OpenFusionExamples/pull/99) | Compare current Windows capabilities on each update, preserve target Runtime representations, and test generic helpers independently. |
| [#100 iOS](https://github.com/defisym/OpenFusionExamples/pull/100) | Thin Runtime adapter, independent common helpers, Mac-to-Windows verified artifacts, independent .ext codec, exported-project integration and distinct validation layers. |
| [#103 focused headers](https://github.com/defisym/OpenFusionExamples/pull/103) | Reuse functional headers and retain compatibility interfaces instead of proliferating replacement utilities. |
| [#91 Encryption](https://github.com/defisym/OpenFusionExamples/pull/91) | One portable implementation and existing C ABI, legacy asset compatibility, reusable module without plugin types. |
| [#104 Fini](https://github.com/defisym/OpenFusionExamples/pull/104) | Original core behind narrow feature macros, independent platform roots, shared encryption runtime, explicit compiled versus export-only adapter fingerprints. Consult actual linkage rather than assuming all client artifacts are dynamic. |
| [#101 media Android](https://github.com/defisym/OpenFusionExamples/pull/101) | Split core changes by functional problem before platform integration; preserve review checkpoints and existing control-flow intent. The preview/queue review demonstrates why a platform port must not casually redesign core behavior. |

The current instructions refine earlier guidance:

- C++ reuse and shared components are requirements, not optional assessments.
- A self-contained core with no external dependencies may be static. Encryption
  is a common module for which a dynamic runtime is preferred for evaluation;
  not every iOS library must be dynamic.
- iOS intermediate projects need a batch builder with explicit dependency closure.
- RunOnly orchestration must be configuration-driven and extensible to new platforms.
- Read the baseline before even preliminary HTML5/future-platform research.

Existing compatibility details are not the desired naming/layout for new code:
Android currently uses ObjectBaseClass.java, ObjectSelectionClass.java and
ObjectCreationClass.java; Windows uses ObjectBase.h, ObjectSelection.h and
ObjectCreation.h. iOS currently stores common headers in Helpers, which also
avoids the SDK's ObjectSelection.h collision on export. Preserve existing callers
while planning a coordinated migration to Windows-aligned names; do not create a
second competing API. Repository logical ownership and SDK export layout are
separate concerns.

Existing FindTheWay static artifacts and scripts remain valid historical examples
for that self-contained core. Updating these skills does not itself rename helpers,
convert artifacts, add a working remote Mac builder, or generalize all publishers.
When implementing a port, close the relevant gaps and report which remain.
