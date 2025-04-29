#pragma once

#include <CS2/Classes/Color.h>
#include <CS2/Constants/ColorConstants.h>

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

    constexpr auto kImmunePlayerGlowAlpha = 40;
    constexpr color::Saturation kPlayerGlowSaturation{0.7f};
    constexpr cs2::Color kPlayerGlowFallbackColor{cs2::kColorWhite};
    constexpr cs2::Color kPlayerGlowColorTeamT{cs2::kColorTeamTT};
    constexpr cs2::Color kPlayerGlowColorTeamCT{cs2::kColorTeamCT};

    constexpr auto kPlayerColors = cs2::kPlayerColors;
}
