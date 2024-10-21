#include <iostream>
#include <unordered_map>
#include <unordered_set>

#pragma once
#ifdef YUNUTY_EXPORTS
#define YUNUTY_API __declspec(dllexport)
#else
#define YUNUTY_API __declspec(dllimport)
#endif

using namespace std;

namespace FMOD
{
    class System;
    class Sound;
    class Channel;
}

namespace yunutyEngine
{
    class SoundChannel;
    class YUNUTY_API SoundSystem
    {
    public:
        static SoundChannel PlaySoundfile(string soundPath);
        // Music을 따로 채널에서 관리한다? 이건 찐빠같은 설계로, 복잡한 게임을 구현하기 위해서는 AudioSource 객체에서 채널 컨트롤이 가능하게 해야한다.
        static SoundChannel PlayMusic(string soundPath);
        static SoundChannel StopMusic(double fadeLength=0);
    private:
        static SoundSystem* soundInstance;
        static SoundSystem* SingleInstance();
        SoundSystem();
        ~SoundSystem();
        SoundChannel mPlaySound(string soundPath);
        // Music을 따로 채널에서 관리한다? 이건 찐빠같은 설계로, 복잡한 게임을 구현하기 위해서는 AudioSource 객체에서 채널 컨트롤이 가능하게 해야한다.
        SoundChannel mPlayMusic(string soundPath);
        SoundChannel mStopMusic(double fadeLength);
        // fire and forget channels
        FMOD::Channel* channels[64] = { nullptr };
        FMOD::Channel* bgmChannel = nullptr;
        FMOD::System* fmodSystem = nullptr;
        void* extradriverdata = 0;
        unordered_map<string, FMOD::Sound*> sounds;
    };
};
