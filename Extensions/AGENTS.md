# Android runtime and Windows helper synchronization

For every Android extension/runtime update, and changes to the Windows object
selection/creation helpers, read and follow
[`android-runtime-parity`](../.agents/skills/android-runtime-parity/SKILL.md).

Check the current Windows helpers for additions or behavior changes on every
such update, synchronize portable behavior to Android, and preserve necessary
platform-specific runtime implementations. The skill includes the comparison
paths, reviewed revision, compatibility requirements and validation workflow.

# Common encryption runtime

For plugin encryption, encrypted-asset integration, or shared encryption build/package
changes, read and follow
[`fusion-shared-encryption`](../.agents/skills/fusion-shared-encryption/SKILL.md).
The encryption runtime is shared by plugins, not privately embedded in each plugin.
For Apple artifacts preserve Mac build/push -> Windows pull -> VS RunOnly packaging.

# Common and iOS porting rules

For any extension port, read
[`fusion-extension-porting`](../.agents/skills/fusion-extension-porting/SKILL.md).
For Apple Runtime, project or publishing changes, also follow
[`fusion-ios-porting`](../.agents/skills/fusion-ios-porting/SKILL.md).
Use independent generic object helpers where applicable, compare their portable
API with Windows/Android, and test them without linking a plugin core.
