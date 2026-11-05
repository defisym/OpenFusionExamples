// JNI marshals platform handles; CopyAdapter owns the rendering operation.
#ifdef __ANDROID__
#include "CopyAdapter.h"
#include <jni.h>
extern "C" JNIEXPORT void JNICALL Java__1DeLib_FFMpegTexture_nBlit(JNIEnv* env, jclass,
    jint program, jint external, jint framebuffer, jint vbo, jint width, jint height, jfloatArray matrix) {
    if (!matrix || env->GetArrayLength(matrix) != 16) return;
    AndroidCopyTarget target;
    target.program = program; target.external = external; target.framebuffer = framebuffer;
    target.vbo = vbo; target.width = width; target.height = height;
    env->GetFloatArrayRegion(matrix, 0, 16, target.transform);
    if (!env->ExceptionCheck()) CopyAdapterFactory(AV_HWDEVICE_TYPE_MEDIACODEC)->CopyTexture(target, nullptr);
}
#endif
