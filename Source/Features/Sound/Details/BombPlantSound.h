#pragma once

#include <string_view>

#include <CS2/Constants/SoundNames.h>

struct BombPlantSound {
    static constexpr auto kFadeAwayStart = 2.1f;
    static constexpr auto kFadeAwayDuration = 0.4f;
    static constexpr auto kMinScale = 0.4f;

    [[nodiscard]] static constexpr bool isSound(std::string_view soundName) noexcept
    {
        return soundName == cs2::kBombPlantSoundPath;
    }
};
