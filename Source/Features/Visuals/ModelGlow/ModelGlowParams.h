#pragma once

#include <CS2/Classes/Color.h>
#include <CS2/Constants/ColorConstants.h>
#include <Utils/ColorUtils.h>

namespace model_glow_params
{
    constexpr cs2::Color kDefuseKitColor{0, 238, 255};
    constexpr cs2::Color kDroppedBombColor{255, 255, 0};
    constexpr cs2::Color kMolotovColor{255, 191, 0};
    constexpr cs2::Color kFlashbangColor{0, 89, 255};
    constexpr cs2::Color kHEGrenadeColor{255, 0, 0};
    constexpr cs2::Color kSmokeGrenadeColor{0, 255, 0};
    constexpr cs2::Color kTickingBombColor{255, 0, 0};
    constexpr cs2::Color kDefaultWeaponColor{cs2::kColorWhite};

    constexpr color::HueInteger kEnemyHue{0};
    constexpr color::HueInteger kAllyHue{120};
    constexpr color::HueInteger kTeamTHue{42};
    constexpr color::HueInteger kTeamCTHue{210};
    constexpr color::HueInteger kPlayerBlueHue{201};
    constexpr color::HueInteger kPlayerGreenHue{166};
    constexpr color::HueInteger kPlayerYellowHue{59};
    constexpr color::HueInteger kPlayerOrangeHue{32};
    constexpr color::HueInteger kPlayerPurpleHue{317};

    constexpr color::Saturation kNormalPlayerColorSaturation{1.0f};
    constexpr color::Saturation kImmunePlayerColorSaturation{0.5f};

}
