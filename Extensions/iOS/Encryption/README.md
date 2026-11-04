# Shared iOS encryption runtime

`iOS/Encryption/build.py` builds a **dynamic** OFEEncryption.framework for device arm64 and
simulator arm64/x86_64, then combines them into OFEEncryption.xcframework.
Only it links libcrypto; Fini.xcframework contains the adapter without crypto.
Add and embed/sign the shared framework once in the final app; link all plugins
against that same framework. Do not merge libcrypto into each plugin archive.
The common library exposes the stable Encryption_* C ABI and keeps EVP symbols
private through an export list. Its install name is
`@rpath/OFEEncryption.framework/OFEEncryption`.

```sh
python3 Extensions/iOS/Encryption/build.py \
  --openssl work/mobile-encryption/openssl/ios \
  --output work/mobile-encryption/ios --prepare-openssl
```

The build stages native handoff inputs into this directory's `Prebuilt` folder.
The author commits/pushes them; Windows packages them through Fini.py. The C++
implementation remains in `_DeLib/Encryption`. Shared dependency preparation uses
`Extensions/_Script/Encryption/prepare_openssl.py`.
