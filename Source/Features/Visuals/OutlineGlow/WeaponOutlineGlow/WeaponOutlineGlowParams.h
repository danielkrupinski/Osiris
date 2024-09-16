#pragma once

#include <CS2/Classes/Color.h>
#include <CS2/Constants/ColorConstants.h>

namespace weapon_outline_glow_params
{
    constexpr auto kRange = 800;
    constexpr auto kColorAlpha = 102;

    constexpr cs2::Color kWeaponColor{cs2::kColorWhite};
    constexpr cs2::Color kMolotovColor{255, 223, 128};
    constexpr cs2::Color kFlashbangColor{128, 172, 255};
    constexpr cs2::Color kHEGrenadeColor{255, 128, 128};
    constexpr cs2::Color kSmokeGrenadeColor{128, 255, 128};
}
