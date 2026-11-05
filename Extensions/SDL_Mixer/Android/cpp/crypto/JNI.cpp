// Thin JNI ownership/marshalling around the same Encryption C API used on Windows.
#include "Encryption/CAPI.h"
#include <jni.h>
#include <array>
#include <memory>
#include <mutex>
#include <unordered_map>
#include <vector>
namespace {
struct Destroy { void operator()(EncryptionCipherContext* p) const { Encryption_CipherDestroy(p); } };
using Cipher = std::unique_ptr<EncryptionCipherContext, Destroy>;
std::mutex mutex;
std::unordered_map<jlong, Cipher> ciphers;
jlong serial = 1;
bool check(JNIEnv* env, EncryptionResult result) {
    if (result == ENCRYPTION_OK) return true;
    const char* type = result == ENCRYPTION_INVALID_PADDING ? "javax/crypto/BadPaddingException"
        : result == ENCRYPTION_INVALID_ARGUMENT ? "java/lang/IllegalArgumentException"
        : result == ENCRYPTION_OUT_OF_MEMORY ? "java/lang/OutOfMemoryError"
        : "java/security/GeneralSecurityException";
    if (!env->ExceptionCheck()) env->ThrowNew(env->FindClass(type), "Shared C++ encryption operation failed");
    return false;
}
bool material(JNIEnv* env, jstring text, std::array<uint8_t, 32>& output) {
    if (!text) return check(env, ENCRYPTION_INVALID_ARGUMENT);
    const jchar* chars = env->GetStringChars(text, nullptr);
    if (!chars) return false;
    struct Release { JNIEnv* env; jstring text; const jchar* chars;
        ~Release() { env->ReleaseStringChars(text, chars); }
    } release{env,text,chars};
    const auto size = env->GetStringLength(text);
    // JNI strings are UTF-16 units, including embedded NUL; native wchar_t and
    // process pointer width must not change the legacy Fusion Win32 mapping.
    std::vector<uint8_t> bytes(static_cast<size_t>(size) * 2);
    for (jsize i = 0; i < size; ++i) { bytes[i*2] = chars[i] & 255; bytes[i*2+1] = chars[i] >> 8; }
    return check(env, Encryption_DeriveKeyAndIvWithSeedWidth(bytes.data(), bytes.size(), 32,
        output.data(), output.data()+16));
}
jbyteArray array(JNIEnv* env, const uint8_t* bytes, size_t size) {
    auto result = env->NewByteArray(static_cast<jsize>(size));
    if (result && size) env->SetByteArrayRegion(result, 0, static_cast<jsize>(size), reinterpret_cast<const jbyte*>(bytes));
    return result;
}
}
#define JNI_NAME(name) Java__1DeLib_MediaCrypto_##name
extern "C" JNIEXPORT jbyteArray JNICALL JNI_NAME(nMaterial)(JNIEnv* env, jclass, jstring text) {
    try { std::array<uint8_t,32> bytes{}; return material(env,text,bytes) ? array(env,bytes.data(),bytes.size()) : nullptr; }
    catch (const std::bad_alloc&) { check(env,ENCRYPTION_OUT_OF_MEMORY); return nullptr; }
}
extern "C" JNIEXPORT jlong JNICALL JNI_NAME(nOpen)(JNIEnv* env, jclass, jstring text) {
    try {
        std::array<uint8_t,32> bytes{}; if (!material(env,text,bytes)) return 0;
        EncryptionCipherContext* context = nullptr;
        if (!check(env,Encryption_CipherCreate(ENCRYPTION_DECRYPT,bytes.data(),bytes.data()+16,&context))) return 0;
        Cipher owned(context);
        std::lock_guard lock(mutex); const auto id = serial++; ciphers.emplace(id,std::move(owned)); return id;
    } catch (const std::bad_alloc&) { check(env,ENCRYPTION_OUT_OF_MEMORY); return 0; }
}
extern "C" JNIEXPORT jbyteArray JNICALL JNI_NAME(nUpdate)(JNIEnv* env, jclass, jlong id, jbyteArray input, jint count) {
    try {
        if (!input || count < 0 || count > env->GetArrayLength(input)) { check(env,ENCRYPTION_INVALID_ARGUMENT); return nullptr; }
        std::vector<uint8_t> bytes(static_cast<size_t>(count)), output(static_cast<size_t>(count)+16);
        if (count) env->GetByteArrayRegion(input,0,count,reinterpret_cast<jbyte*>(bytes.data()));
        if (env->ExceptionCheck()) return nullptr;
        std::lock_guard lock(mutex); auto found = ciphers.find(id);
        if (found == ciphers.end()) { check(env,ENCRYPTION_INVALID_STATE); return nullptr; }
        size_t size = 0;
        return check(env,Encryption_CipherUpdate(found->second.get(),bytes.data(),bytes.size(),output.data(),output.size(),&size))
            ? array(env,output.data(),size) : nullptr;
    } catch (const std::bad_alloc&) { check(env,ENCRYPTION_OUT_OF_MEMORY); return nullptr; }
}
extern "C" JNIEXPORT jbyteArray JNICALL JNI_NAME(nFinish)(JNIEnv* env, jclass, jlong id) {
    std::lock_guard lock(mutex); auto found = ciphers.find(id);
    if (found == ciphers.end()) { check(env,ENCRYPTION_INVALID_STATE); return nullptr; }
    std::array<uint8_t,16> output{}; size_t size = 0;
    return check(env,Encryption_CipherFinal(found->second.get(),output.data(),output.size(),&size)) ? array(env,output.data(),size) : nullptr;
}
extern "C" JNIEXPORT void JNICALL JNI_NAME(nClose)(JNIEnv*, jclass, jlong id) {
    std::lock_guard lock(mutex); ciphers.erase(id);
}
