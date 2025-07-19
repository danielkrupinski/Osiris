#pragma once

#include <CS2/Classes/Color.h>
#include <Utils/ColorUtils.h>

namespace outline_glow_params
{
    constexpr auto kDefuseKitGlowRange = 800;
    constexpr auto kWeaponGlowRange = 800;

    constexpr auto kGlowAlpha = 102;
    constexpr auto kImmunePlayerGlowAlpha = 40;
    constexpr color::Saturation kSaturation{0.5f};
    constexpr color::Brightness kBrightness{1.0f};

    constexpr color::HueInteger kHostageHue{50};
    constexpr color::HueInteger kDroppedBombHue{60};
    constexpr color::HueInteger kTickingBombHue{0};
    constexpr color::HueInteger kDefuseKitHue{184};
    constexpr color::HueInteger kMolotovHue{45};
    constexpr color::HueInteger kFlashbangHue{219};
    constexpr color::HueInteger kHEGrenadeHue{0};
    constexpr color::HueInteger kSmokeGrenadeHue{120};

    constexpr color::HueInteger kTeamTHue{35};
    constexpr color::HueInteger kTeamCTHue{220};

    constexpr color::HueInteger kPlayerBlueHue{215};
    constexpr color::HueInteger kPlayerGreenHue{125};
    constexpr color::HueInteger kPlayerYellowHue{53};
    constexpr color::HueInteger kPlayerOrangeHue{15};
    constexpr color::HueInteger kPlayerPurpleHue{265};

    constexpr cs2::Color kFallbackColor{191, 191, 191, kGlowAlpha};
}
