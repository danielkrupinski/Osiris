#pragma once

#include <algorithm>

template <typename SoundType>
struct SoundVisualization {
    [[nodiscard]] static constexpr float getScale(float clipSpaceZ) noexcept
    {
        return (std::max)(1.0f - clipSpaceZ / 1000.0f, SoundType::kMinScale);
    }

    [[nodiscard]] static constexpr float getOpacity(float timeAlive) noexcept
    {
        if (timeAlive >= SoundType::kFadeAwayStart) {
            return 1.0f - (std::min)((timeAlive - SoundType::kFadeAwayStart) / SoundType::kFadeAwayDuration, 1.0f);
        } else {
            return 1.0f;
        }
    }
};
