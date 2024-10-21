#include "YunutyEngine.h"
#include "SoundSystem.h"
#include "fmod.h"
#include "SoundChannel.h"
#include "fmod.hpp"
#include <iostream>

using namespace FMOD;

SoundSystem* SoundSystem::soundInstance = nullptr;
SoundChannel yunutyEngine::SoundSystem::PlaySoundfile(string soundPath)
{
    return SingleInstance()->mPlaySound(soundPath);
}
SoundSystem* yunutyEngine::SoundSystem::SingleInstance()
{
    if (!soundInstance)
        soundInstance = new SoundSystem();
    return soundInstance;
}
SoundSystem::SoundSystem()
{
    FMOD::System_Create(&fmodSystem);
    fmodSystem->init(64 + 1, FMOD_INIT_NORMAL, extradriverdata);
}
SoundSystem::~SoundSystem()
{
    fmodSystem->release();
}
SoundChannel yunutyEngine::SoundSystem::mPlaySound(string soundPath)
{
    static int lastChannelIndex = 0;

    if (sounds.find(soundPath) == sounds.end())
    {
        sounds[soundPath] = nullptr;
        fmodSystem->createSound(soundPath.c_str(), FMOD_LOOP_OFF, 0, &sounds[soundPath]);
    }
    for (int i = 0; i < 64; i++)
    {
        bool isPlaying;
        lastChannelIndex = (lastChannelIndex + 1) % 64;
        channels[lastChannelIndex]->isPlaying(&isPlaying);
        if (!isPlaying)
        {
            fmodSystem->playSound(sounds[soundPath], 0, false, &channels[lastChannelIndex]);
            break;
        }
    }
    return SoundChannel(channels[lastChannelIndex]);
}
SoundChannel yunutyEngine::SoundSystem::PlayMusic(string soundPath)
{
    return SoundSystem::SingleInstance()->mPlayMusic(soundPath);
}
SoundChannel yunutyEngine::SoundSystem::StopMusic(double fadeLength)
{
    return SoundSystem::SingleInstance()->mStopMusic(fadeLength);
}
SoundChannel yunutyEngine::SoundSystem::mPlayMusic(string soundPath)
{
    if (sounds.find(soundPath) == sounds.end())
    {
        sounds[soundPath] = nullptr;
        fmodSystem->createSound(soundPath.c_str(), FMOD_LOOP_NORMAL, 0, &sounds[soundPath]);
    }
    bool isPlaying;
    bgmChannel->isPlaying(&isPlaying);
    if (!isPlaying)
        fmodSystem->playSound(sounds[soundPath], 0, false, &bgmChannel);

    return SoundChannel(bgmChannel);
}
SoundChannel yunutyEngine::SoundSystem::mStopMusic(double fadeLength)
{
    bgmChannel->stop();
    return SoundChannel(bgmChannel);
}
