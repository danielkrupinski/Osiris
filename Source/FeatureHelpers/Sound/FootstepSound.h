#pragma once

#include <algorithm>
#include <cassert>

struct FootstepSound {
    static constexpr auto kLifespan = 2.0f;

    [[nodiscard]] static constexpr float getScale(float clipSpaceZ) noexcept
    {
        return (std::max)(1.0f - clipSpaceZ / 1000.0f, 0.3f);
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
        if (soundName.starts_with(cs2::kPlayerFootstepSoundsPath)) {
            return !std::string_view{ soundName.data() + cs2::kPlayerFootstepSoundsPath.length(), soundName.length() - cs2::kPlayerFootstepSoundsPath.length() }.starts_with(cs2::kPlayerSuitSoundPrefix);
        }
        return false;
    }

private:
    static constexpr auto kFadeAwayDuration = 0.4f;
};
