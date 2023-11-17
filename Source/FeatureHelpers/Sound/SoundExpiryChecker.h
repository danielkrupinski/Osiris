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
        return !sound.isAlive(curtime, lifeSpan);
    }

    [[nodiscard]] bool operator()(int guid) const noexcept
    {
        return !hasChannelWithGuid(guid);
    }

private:
    [[nodiscard]] bool hasChannelWithGuid(int guid) const noexcept
    {
        for (int i = 0; i < channelInfo.size; ++i) {
            if (const auto& channel = channelInfo.memory[i]; channel.sfx && channel.guid == guid)
                return true;
        }
        return false;
    }

    const cs2::CUtlVector<cs2::ChannelInfo1>& channelInfo;
    float curtime;
    float lifeSpan;
};
