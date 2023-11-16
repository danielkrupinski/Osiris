#pragma once

#include <algorithm>
#include <cassert>
#include <string_view>

#include <CS2/Constants/SoundNames.h>

struct BombBeepSound {
    static constexpr auto kFadeAwayStart = 0.0f;
    static constexpr auto kFadeAwayDuration = 0.3f;

    [[nodiscard]] static constexpr float getScale(float clipSpaceZ) noexcept
    {
        return (std::max)(1.0f - clipSpaceZ / 1000.0f, 0.4f);
    }

    [[nodiscard]] static constexpr float getOpacity(float timeAlive) noexcept
    {
        if (timeAlive >= kFadeAwayStart) {
            return 1.0f - (std::min)((timeAlive - kFadeAwayStart) / kFadeAwayDuration, 1.0f);
        } else {
            return 1.0f;
        }
    }

    [[nodiscard]] static constexpr bool isSound(std::string_view soundName) noexcept
    {
        return soundName.starts_with(cs2::kBombSoundsPath) && std::string_view{ soundName.data() + cs2::kBombSoundsPath.length(), soundName.length() - cs2::kBombSoundsPath.length() }.starts_with(cs2::kBombBeepSoundsPrefix);
    }
};
