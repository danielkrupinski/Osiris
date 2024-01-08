#pragma once

#include <algorithm>

template <typename SoundType>
struct SoundVisualization {
    [[nodiscard]] static constexpr float getScale(float z, float fovScale) noexcept
    {
        constexpr auto kMaxScale{1.0f};
        return std::clamp(500.0f / (z / fovScale + 400.0f), SoundType::kMinScale, kMaxScale);
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
