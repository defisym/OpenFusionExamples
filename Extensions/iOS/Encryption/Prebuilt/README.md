# Common author-built Apple runtime

The Mac common build stages OFEEncryption.xcframework.zip and manifest.json here.
Commit/push both once for all dependent plugins. Windows packaging checks hashes,
source freshness and required slices before updating OFEEncryption-iOS.zip.
Do not duplicate these binaries in each plugin's Prebuilt directory.
