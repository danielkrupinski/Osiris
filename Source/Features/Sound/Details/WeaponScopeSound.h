#pragma once

#include <string_view>

#include <CS2/Constants/SoundNames.h>

struct WeaponScopeSound {
    static constexpr auto kFadeAwayStart = 1.8f;
    static constexpr auto kFadeAwayDuration = 0.8f;
    static constexpr auto kMinScale = 0.5f;

    [[nodiscard]] static constexpr bool isSound(std::string_view soundName) noexcept
    {
        return soundName.starts_with(cs2::kWeaponSoundsPath) && soundName.find(cs2::kZoomSoundNamePart) != std::string_view::npos;
    }
};
