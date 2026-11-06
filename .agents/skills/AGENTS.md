# Skill authoring conventions

- Write all skills, bundled references, and related porting documentation in English.
- State each skill's platform scope explicitly; keep Android and iOS procedures separate.
- Describe reusable workflows in terms of the target extension. Named extensions may be optional examples, never mandatory dependencies for unrelated ports.
- Obtain build commands, versions, artifact names, and paths from the target extension's current scripts and configuration. Do not treat one extension's settings as repository-wide defaults.

- Keep `fusion-extension-porting` as the mandatory entry point for every platform,
  including preliminary HTML5/future-platform research. Platform skills add
  details without relaxing its C++ reuse, layout, common-component, dependency
  build or functional PR-order requirements.
- Distinguish standing requirements from historical implementation examples.
  Changes to a skill alone do not establish that scripts or binaries migrated.
