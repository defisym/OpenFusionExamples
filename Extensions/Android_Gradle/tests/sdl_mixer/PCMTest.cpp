#include "AudioRuntime.h"
#include <SDL.h>
#include <SDL_mixer.h>
#include <atomic>
#include <algorithm>
#include <cmath>
#include <iostream>
#include <stdexcept>
#include <vector>
struct Capture {
    std::atomic<size_t> written{0};
    std::vector<int16_t> samples = std::vector<int16_t>(44100*3);
    static void SDLCALL callback(void* user, Uint8* data, int bytes) {
        auto& self=*static_cast<Capture*>(user);auto source=reinterpret_cast<int16_t*>(data);
        size_t start=self.written.load(std::memory_order_relaxed);
        size_t count=std::min<size_t>(bytes/4,self.samples.size()-std::min(start,self.samples.size()));
        for(size_t i=0;i<count;++i)self.samples[start+i]=source[i*2];
        self.written.store(start+count,std::memory_order_release);
    }
};
void check(bool ok,const char* name){if(!ok)throw std::runtime_error(name);}
int main(){
    SDL_setenv("SDL_AUDIODRIVER","dummy",1);
    auto client=OFEAudio_Acquire();check(client!=0,"audio acquisition");
    void* pcm=OFEAudio_CreatePCM();Capture capture;Mix_SetPostMix(Capture::callback,&capture);
    std::vector<int16_t> wave(44100*2);
    for(size_t i=0;i<wave.size()/2;++i)wave[i*2]=wave[i*2+1]=static_cast<int16_t>(8000*std::sin(i*440*6.283185307/44100));
    OFEAudio_PCMWrite(pcm,wave.data(),wave.size()*2,0);OFEAudio_PCMState(pcm,1,1,1);
    SDL_Delay(160);check(OFEAudio_PCMClock(pcm)>80,"device callback clock");
    OFEAudio_PCMState(pcm,0,1,1);double stopped=OFEAudio_PCMClock(pcm);SDL_Delay(50);
    check(OFEAudio_PCMClock(pcm)==stopped,"per-stream pause");
    OFEAudio_PCMReset(pcm,0);OFEAudio_PCMState(pcm,0,1,2);
    OFEAudio_SetSuspended(client,1);capture.written=0;OFEAudio_SetSuspended(client,0);
    OFEAudio_PCMWrite(pcm,wave.data(),wave.size()*2,0);OFEAudio_PCMFlush(pcm);OFEAudio_PCMState(pcm,1,1,2);
    for(int i=0;i<200&&OFEAudio_PCMPending(pcm);++i){OFEAudio_PCMFlush(pcm);SDL_Delay(10);}
    check(OFEAudio_PCMPending(pcm)==0,"tempo tail drain");
    OFEAudio_PCMState(pcm,0,1,2);
    OFEAudio_SetSuspended(client,1);size_t count=capture.written.load();
    int crossings=0;size_t first=count,last=0;
    for(size_t i=1;i<count;++i){if(std::abs(capture.samples[i])>200){first=std::min(first,i);last=i;}if(capture.samples[i-1]<=0&&capture.samples[i]>0)++crossings;}
    check(last>first,"PCM produces non-silent output through Mixer X");
    double seconds=(last-first)/44100.;double frequency=crossings/seconds;
    check(seconds>.35&&seconds<.7,"tempo changes duration");
    check(frequency>410&&frequency<470,"tempo preserves pitch");
    OFEAudio_PCMReset(pcm,1000);check(OFEAudio_PCMPending(pcm)==0&&OFEAudio_PCMClock(pcm)==1000,"seek discards old samples");
    Mix_SetPostMix(nullptr,nullptr);OFEAudio_DestroyPCM(pcm);OFEAudio_Release(client);
    std::cout<<"PASS: actual mixed PCM, pause, reset, bounded queue, tempo duration/pitch and DSP tail ("<<frequency<<" Hz)\n";
}
