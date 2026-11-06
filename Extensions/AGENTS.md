# Mandatory baseline for every platform port

Before feasibility research, design, prototypes or implementation for any platform
(including HTML5 and future platforms), read and follow
[`fusion-extension-porting`](../.agents/skills/fusion-extension-porting/SKILL.md)
and its build contract, then the applicable platform skill. Reuse the original
C++ core, common libraries and existing build/publishing tools. Do not start an
independent implementation before checking these contracts. Platform details
refine the baseline; an undecided platform does not waive it.

For Android work, also read
[`fusion-android-porting`](../.agents/skills/fusion-android-porting/SKILL.md).
For iOS work, also read
[`fusion-ios-porting`](../.agents/skills/fusion-ios-porting/SKILL.md).
Split PR commits by functional logic, C++ core changes first and platform
integration last. See the baseline for shared component, naming and directory
requirements and configuration-driven RunOnly publishing.

# Android runtime and Windows helper synchronization

For every Android extension/runtime update, and changes to the Windows object
selection/creation helpers, read and follow
[`android-runtime-parity`](../.agents/skills/android-runtime-parity/SKILL.md).

Check the current Windows helpers for additions or behavior changes on every
such update, synchronize portable behavior to Android, and preserve necessary
platform-specific runtime implementations. The skill includes the comparison
paths, reviewed revision, compatibility requirements and validation workflow.

# iOS Runtime object helpers

For iOS Runtime and object helper changes, follow
[`fusion-ios-porting`](../.agents/skills/fusion-ios-porting/SKILL.md).
Keep generic ObjectBase, ObjectSelection and ObjectCreation classes separate
from plugin adapters. Compare their portable APIs with the current Windows and
Android helpers, and validate generic helpers without linking a plugin core.
