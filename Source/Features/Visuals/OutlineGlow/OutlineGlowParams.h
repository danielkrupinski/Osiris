#pragma once

#include <CS2/Classes/Color.h>

namespace outline_glow_params
{
    constexpr auto kGlowAlpha = 102;
    constexpr cs2::Color kHostageColor{255, 234, 128, kGlowAlpha};
    constexpr cs2::Color kDroppedBombColor{255, 255, 128, kGlowAlpha};
    constexpr cs2::Color kTickingBombColor{255, 128, 128, kGlowAlpha};
    constexpr auto kDefuseKitGlowRange = 800;
    constexpr cs2::Color kDefuseKitColor{127, 247, 255, kGlowAlpha};
    constexpr auto kWeaponGlowRange = 800;
    constexpr cs2::Color kMolotovColor{255, 223, 128, kGlowAlpha};
    constexpr cs2::Color kFlashbangColor{128, 172, 255, kGlowAlpha};
    constexpr cs2::Color kHEGrenadeColor{255, 128, 128, kGlowAlpha};
    constexpr cs2::Color kSmokeGrenadeColor{128, 255, 128, kGlowAlpha};
    constexpr cs2::Color kDefaultWeaponColor{191, 191, 191, kGlowAlpha};
}
