#include "fmod.hpp"
#include "SoundChannel.h"

void yunutyEngine::SoundChannel::StopSound()
{
    channel->stop();
    //bool isPlaying;
    //channel->isPlaying(&isPlaying);
    //return channel->isPlaying();
}
yunutyEngine::SoundChannel::SoundChannel(FMOD::Channel* channel)
    :channel(channel)
{
}
