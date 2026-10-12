#pragma once

#include <limits>

#include <CS2/Classes/Vector.h>
#include <CS2/Classes/EntitySystem/CEntityHandle.h>
#include <CS2/Constants/EntityHandle.h>

struct PlayedSound {
    float spawnTime;
    cs2::Vector origin;
    cs2::CEntityHandle source{cs2::INVALID_EHANDLE_INDEX};
    float firstSpawnTime{-1.0f};

    [[nodiscard]] float getTimeAlive(float curtime) const noexcept
    {
        if (curtime >= spawnTime)
            return curtime - spawnTime;
        return (std::numeric_limits<float>::max)();
    }

    [[nodiscard]] bool isAlive(float curtime, float lifetime) const noexcept
    {
        return getTimeAlive(curtime) <= lifetime;
    }
};
