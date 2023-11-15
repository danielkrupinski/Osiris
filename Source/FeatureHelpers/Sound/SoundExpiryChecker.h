#pragma once

#include <CS2/Classes/CUtlVector.h>
#include <CS2/Classes/Sound.h>

#include "PlayedSound.h"

struct SoundExpiryChecker {
    SoundExpiryChecker(const cs2::CUtlVector<cs2::ChannelInfo1>& channelInfo, float curtime, float lifeSpan) noexcept
        : channelInfo{ channelInfo }
        , curtime{ curtime }
        , lifeSpan{ lifeSpan }
    {
    }

    [[nodiscard]] bool operator()(const PlayedSound& sound) const noexcept
    {
        if (sound.isAlive(curtime, lifeSpan))
            return false;

        for (int i = 0; i < channelInfo.size; ++i) {
            if (const auto& channel = channelInfo.memory[i]; channel.sfx && channel.guid == sound.guid)
                return false;
        }
        return true;
    }

private:
    const cs2::CUtlVector<cs2::ChannelInfo1>& channelInfo;
    float curtime;
    float lifeSpan;
};
