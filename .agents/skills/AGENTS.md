# Skill authoring conventions

- Write all skills, bundled references, and related porting documentation in English.
- State each skill's platform scope explicitly; keep Android and iOS procedures separate.
- Describe reusable workflows in terms of the target extension. Named extensions may be optional examples, never mandatory dependencies for unrelated ports.
- Obtain build commands, versions, artifact names, and paths from the target extension's current scripts and configuration. Do not treat one extension's settings as repository-wide defaults.
- Every extension/platform skill must preserve the standing repository-layout rule in fusion-extension-porting/SKILL.md: adapt existing libraries in place with narrow macros or in the corresponding platform component directory. Apply this to future ports automatically; do not invent mobile subtrees in Windows plugin/library projects.
