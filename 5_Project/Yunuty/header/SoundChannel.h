#pragma once
#include <iostream>

#ifdef YUNUTY_EXPORTS
#define YUNUTY_API __declspec(dllexport)
#else
#define YUNUTY_API __declspec(dllimport)
#endif

namespace FMOD
{
    class Channel;
}
using namespace std;
namespace yunutyEngine
{
    class SoundSystem;
    class YUNUTY_API SoundChannel
    {
    public:
        void StopSound();
    private:
        SoundChannel(FMOD::Channel* channel);
        FMOD::Channel* channel;
        friend yunutyEngine::SoundSystem;
    };
};
