# Author-built Mac artifacts

FiniXCFramework stages Fini.xcframework.zip and manifest.json here after a successful
Mac build. Commit/push both files together; do not add placeholders. Windows pull
and VS RunOnly's Fini.py validate them and generate/update Fini-iOS.zip automatically.
The shared runtime is committed separately in Extensions/iOS/Encryption/Prebuilt.
No exporter SDK source belongs here.
