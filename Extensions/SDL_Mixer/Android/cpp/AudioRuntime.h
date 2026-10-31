#pragma once
#include <stdint.h>
#include <stddef.h>
#ifdef __cplusplus
extern "C" {
#endif
// One implementation in libofe_audio.so; all native plugins dynamically depend on it.
uint64_t OFEAudio_Acquire(void);
void OFEAudio_Release(uint64_t client);
void OFEAudio_SetSuspended(uint64_t client, int suspended);
void OFEAudio_SetFocus(int focused);
int OFEAudio_GetSpec(int* frequency, uint16_t* format, int* channels);
unsigned OFEAudio_ABIVersion(void);
// Native-only owned PCM stream. Device acquisition outlives the stream.
// Control/write methods may throw C++ exceptions, caught at the JNI boundary.
void* OFEAudio_CreatePCM(void);
// Pull mode lets existing C++ engines supply their PCM directly. Callback output is
// S16 stereo at 44100 Hz; the transport only converts to the actual device rate.
// No decoder, media clock or tempo policy is installed in this transport.
typedef void (*OFEAudio_PCMCallback)(void* user, void* bytes, int count);
void* OFEAudio_CreatePullPCM(OFEAudio_PCMCallback callback, void* user);
void OFEAudio_DestroyPCM(void* stream);
void OFEAudio_PCMWrite(void* stream, const void* bytes, size_t count, double timestamp_ms);
void OFEAudio_PCMFlush(void* stream);
void OFEAudio_PCMReset(void* stream, double position_ms);
void OFEAudio_PCMState(void* stream, int running, float volume, double tempo);
double OFEAudio_PCMClock(void* stream);
size_t OFEAudio_PCMPending(void* stream);
#ifdef __cplusplus
}
#endif
