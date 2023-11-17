#pragma once

#include <limits>

#include <CS2/Classes/Vector.h>

struct PlayedSound {
    float spawnTime;
    cs2::Vector origin;

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
