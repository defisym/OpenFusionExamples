#include "IniCore.h"
#include <jni.h>
#include <mutex>
#include <unordered_map>
#include <atomic>

namespace {
std::mutex handlesMutex;
std::unordered_map<jlong,std::shared_ptr<fini::State>> handles;
std::atomic<jlong> nextHandle{1};
std::shared_ptr<fini::State> retain(jlong handle) {
    std::lock_guard<std::mutex> lock(handlesMutex);
    auto it=handles.find(handle);fini::check(it!=handles.end(),"Closed or invalid Fini handle");return it->second;
}
std::string text(JNIEnv* env,jstring value) {
    fini::check(value!=nullptr,"Null JNI string");
    const auto* units=env->GetStringChars(value,nullptr);
    fini::check(units!=nullptr,"Read JNI string");
    std::u16string copy;
    try {copy.assign(reinterpret_cast<const char16_t*>(units),env->GetStringLength(value));}
    catch(...){env->ReleaseStringChars(value,units);throw;}
    env->ReleaseStringChars(value,units);
    return std::wstring_convert<std::codecvt_utf8_utf16<char16_t>,char16_t>{}.to_bytes(copy);
}
void fail(JNIEnv* env,const std::exception& error) {
    if(env->ExceptionCheck())return;
    auto cls=env->FindClass("java/lang/IllegalArgumentException");
    if(cls){env->ThrowNew(cls,error.what());env->DeleteLocalRef(cls);}
}
}
extern "C" JNIEXPORT jlong JNICALL Java__1DeLib_FiniNative_create(JNIEnv* env,jclass) {
    try {auto state=std::make_shared<fini::State>();auto handle=nextHandle.fetch_add(1);std::lock_guard<std::mutex> lock(handlesMutex);handles.emplace(handle,std::move(state));return handle;}
    catch(const std::exception& e){fail(env,e);return 0;}
}
extern "C" JNIEXPORT void JNICALL Java__1DeLib_FiniNative_destroy(JNIEnv*,jclass,jlong handle) {
    std::lock_guard<std::mutex> lock(handlesMutex);handles.erase(handle);
}
extern "C" JNIEXPORT jstring JNICALL Java__1DeLib_FiniNative_request(JNIEnv* env,jclass,jlong handle,jstring op,jstring args) {
    try {
        auto state=retain(handle);
        // All requests run on Fusion's runtime thread; no callbacks occur while native state is in use.
        auto result=state->request(text(env,op),fini::Json::parse(text(env,args)));
        auto units=to_utf16_string(result.dump());
        return env->NewString(reinterpret_cast<const jchar*>(units.data()),units.size());
    }catch(const std::exception& e){fail(env,e);return nullptr;}
}
