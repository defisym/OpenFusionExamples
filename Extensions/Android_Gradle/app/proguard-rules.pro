# Add project specific ProGuard rules here.
# You can control the set of applied configuration files using the
# proguardFiles setting in build.gradle.
#
# For more details, see
#   http://developer.android.com/guide/developing/tools/proguard.html

# If your project uses WebView with JS, uncomment the following
# and specify the fully qualified class name to the JavaScript interface
# class:
-keepclassmembers class Extensions.CRunWebView2$MyJavaScriptInterface {
   public *;
}

#-optimizationpasses 5
#-dontusemixedcaseclassnames
#-dontskipnonpubliclibraryclasses
#-dontpreverify
#-verbose

-keep, allowobfuscation class Actions.*
-keep, allowobfuscation class Conditions.*
-keep, allowobfuscation class Expressions.*
-keep, allowobfuscation class Events.*
-keep, allowobfuscation class Frame.*
-keep, allowobfuscation class Extensions.*
-keep, allowobfuscation class Objects.*
-keep, allowobfuscation class Sprites.*
-keep, allowobfuscation class Services.*
-keep, allowobfuscation class Transitions.*
-keep, allowobfuscation class Values.*
-keep, allowobfuscation class RunLoop.*
-keep, allowobfuscation class Runtime.*

-keepclassmembers, allowobfuscation class * {
    *;
}

-keepnames class *
-keepclassmembernames class * {
    public <methods>;
    public <fields>;
}

-keepclasseswithmembernames class Banks.CImage {
native <methods>;
}

-keepclasseswithmembernames class OpenGL.ES1Renderer {
native <methods>;
}

-keepclasseswithmembernames class OpenGL.ES2Renderer {
native <methods>;
}

-keepclasseswithmembernames class OpenGL.ES3Renderer {
 native <methods>;
}

-keepclasseswithmembernames class Sprites.CMask {
 native <methods>;
}

-keepclasseswithmembernames class Sprites.CColMask {
  native <methods>;
}

#-keep class sun.misc.Unsafe { *; }
-keep class com.android.vending.billing.**
-keep class com.google.gson.stream.** { *; }

# Uncomment this to preserve the line number information for
# debugging stack traces.
-keepattributes SourceFile,LineNumberTable

# If you keep the line number information, uncomment this to
# hide the original source file name.
-renamesourcefileattribute SourceFile
