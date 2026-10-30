# Android iconv build dependency

Pinned GNU libiconv source, licenses and per-ABI static archives for FindTheWay. No Fusion SDK files.
The publishing script verifies every archive/header and downloads the pinned corresponding source automatically.
The source is distributed with the generated package, including the build version of aliases.h (Windows INT_PTR aliases changed to long for Android).
Built with NDK r28c, API 21: configure --host=<target> --disable-shared --enable-static --disable-nls; CC=<target>21-clang, AR=llvm-ar, RANLIB=llvm-ranlib, CFLAGS="-O2 -fPIC"; make and make install.
Targets: aarch64-linux-android, arm-linux-androideabi (CC armv7a-linux-androideabi21-clang), i686-linux-android and x86_64-linux-android.
For a source rebuild replace source/lib/aliases.h with aliases.h.build-version before building.
