#include "Mixer.h"
#include <atomic>
#include <cstdlib>
#include <iostream>
#include <stdexcept>
#include <fstream>
using ofe::audio::Mixer;
void check(bool value, const char* name) { if (!value) throw std::runtime_error(name); }
std::atomic<uint64_t> left{0}, right{0}, crossings{0};
void SDLCALL capture(void*, Uint8* bytes, int length) {
    auto pcm = reinterpret_cast<const int16_t*>(bytes); int previous = 0;
    uint64_t l = 0, r = 0, c = 0;
    for (int i = 0; i + 1 < length / 2; i += 2) { l += std::abs(static_cast<int>(pcm[i])); r += std::abs(static_cast<int>(pcm[i+1])); if (previous <= 0 && pcm[i] > 0) ++c; previous = pcm[i]; }
    left.fetch_add(l); right.fetch_add(r); crossings.fetch_add(c);
}
int main(int argc, char** argv) {
    check(argc >= 5, "audio codec fixtures required");
    auto first = OFEAudio_Acquire(), second = OFEAudio_Acquire();
    check(first && second && first != second, SDL_GetError());
    check(OFEAudio_ABIVersion() == 2, "audio ABI");
    OFEAudio_Release(first); check(OFEAudio_GetSpec(nullptr,nullptr,nullptr), "second client retains device");
    Mix_SetPostMix(capture, nullptr);
    {
        Mixer mixer;
        for (int i = 1; i < argc; ++i) {
            mixer.play("fixture", argv[i], 0, true, 1, -1); SDL_Delay(50);
            check(mixer.condition(0,0,true), "native codec playback");
            check(mixer.number(3,0,true) > 1, "duration");
        }
        mixer.action(7,0,true,-1); mixer.update(); check(mixer.condition(1,0,true), "stop all");
        mixer.action(2,0,true,80); mixer.action(13,0,true,50);
        check(mixer.number(0,0,true) == 80 && mixer.number(9,0,true) == 50, "channel/master settings");
        mixer.play("old",argv[1],0,true,0,-1); mixer.play("new",argv[1],0,true,0,100);
        check(mixer.liveVoices().size() == 2, "crossfade retains outgoing stream");
        SDL_Delay(180); mixer.update(); check(mixer.liveVoices().size() == 1, "crossfade cleanup");
        check(mixer.channelName(0) == "new", "exclusive replacement");
        mixer.action(4,0,true); mixer.update(); check(mixer.condition(4,0,true), "all paused");
        auto position = mixer.number(2,0,true); SDL_Delay(40);
        check(mixer.number(2,0,true) == position, "user pause preserves playhead");
        OFEAudio_SetSuspended(second,1); OFEAudio_SetSuspended(second,0);
        check(mixer.condition(3,0,true), "app resume preserves user pause");
        mixer.play("resume replacement",argv[1],0,true,0,100);mixer.update();
        check(mixer.liveVoices().size()==1, "paused outgoing stream reclaimed on replacement");
        mixer.action(5,0,true); mixer.action(6,0,true,.4); check(mixer.number(2,0,true) >= .35, "stream seek");
        mixer.action(8,0,true,.05,.1); SDL_Delay(140); mixer.update(); check(mixer.number(2,0,true) < .15, "AB loop");
        try { mixer.play("bad",argv[1],-1,true,1,-1); throw std::runtime_error("negative channel accepted"); } catch (const std::invalid_argument&) {}
        mixer.action(7,0,true,-1); mixer.update();
        mixer.cache("bank",argv[1],false);
        std::ifstream file(argv[1],std::ios::binary|std::ios::ate); auto size=static_cast<int64_t>(file.tellg());
        auto view=mixer.memoryName("memory", "bank",0,size); mixer.play(view,"",3,true,1,-1);
        check(!mixer.unreferenced("bank") && mixer.channelName(3)=="memory", "handled binary reference");
        mixer.erase("bank"); check(!mixer.memoryName("still present","bank",0,size).empty(), "active binary retained");
        mixer.cache("bank",argv[1],true); check(!mixer.unreferenced("bank"), "update retains old readers");
        mixer.action(7,0,true,-1); mixer.update(); mixer.erase("bank");
        check(mixer.memoryName("gone","bank",0,size).empty(), "unreferenced binary release");
        mixer.play("partial",argv[1],4,false,0,-1); mixer.action(4,4,false);
        mixer.play("partial",argv[1],4,false,0,-1);
        check(!mixer.condition(5,4,false), "mixed paused and audible voices still have output");
        mixer.action(3,4,false,-1); mixer.update();
        mixer.action(13,0,true,100);
        mixer.action(2,1,false,0); mixer.play("tone",argv[1],1,false,0,-1);
        mixer.action(9,2,false,1,0,"{0,12}"); mixer.action(14,2,false,255,0);
        check(mixer.number(10,2,false)==255 && mixer.number(11,2,false)==0, "panning ACE order");
        mixer.play("tone",argv[1],2,false,0,-1);
        SDL_Delay(200); left=right=crossings=0; SDL_Delay(300);
        check(left.load()>0 && right.load()<left.load()/20, "panning applied to new stream");
        check(crossings.load()>190 && crossings.load()<400, "SoundTouch octave pitch through real mixer");
        check(!mixer.condition(4,0,true) && !mixer.condition(5,2,false), "aggregate pause and mixing output conditions");
        mixer.action(7,0,true,-1); mixer.update(); check(mixer.liveVoices().empty(), "all voices released");
    }
    Mix_SetPostMix(nullptr,nullptr); OFEAudio_Release(second);
    check(!OFEAudio_GetSpec(nullptr,nullptr,nullptr), "last client closes device");
    std::cout << "PASS: real Mixer X WAV/MP3/Ogg/FLAC/Opus/WavPack codecs, crossfade, seek/AB, pause, shared lifetime, cache, panning and SoundTouch\n";
}
