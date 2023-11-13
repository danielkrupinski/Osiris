#pragma once

#include <algorithm>
#include <cassert>

struct BombPlantSound {
    static constexpr auto kLifespan = 2.5f;

    [[nodiscard]] static constexpr float getScale(float clipSpaceZ) noexcept
    {
        return (std::max)(1.0f - clipSpaceZ / 1000.0f, 0.4f);
    }

    [[nodiscard]] static constexpr float getOpacity(float timeAlive) noexcept
    {
        assert(timeAlive <= kLifespan);
        if (kLifespan - timeAlive <= kFadeAwayDuration) {
            return (kLifespan - timeAlive) / kFadeAwayDuration;
        } else {
            return 1.0f;
        }
    }

    [[nodiscard]] static constexpr bool isSound(std::string_view soundName) noexcept
    {
        return soundName == cs2::kBombPlantSoundPath;
    }

private:
    static constexpr auto kFadeAwayDuration = 0.4f;
};
