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

    constexpr color::HueInteger kPlayerBlueMinHue{191};
    constexpr color::HueInteger kPlayerBlueDefaultHue{215};
    constexpr color::HueInteger kPlayerBlueMaxHue{240};

    constexpr color::HueInteger kPlayerGreenMinHue{110};
    constexpr color::HueInteger kPlayerGreenDefaultHue{125};
    constexpr color::HueInteger kPlayerGreenMaxHue{140};

    constexpr color::HueInteger kPlayerYellowMinHue{47};
    constexpr color::HueInteger kPlayerYellowDefaultHue{53};
    constexpr color::HueInteger kPlayerYellowMaxHue{60};
    
    constexpr color::HueInteger kPlayerOrangeMinHue{11};
    constexpr color::HueInteger kPlayerOrangeDefaultHue{15};
    constexpr color::HueInteger kPlayerOrangeMaxHue{20};

    constexpr color::HueInteger kPlayerPurpleMinHue{250};
    constexpr color::HueInteger kPlayerPurpleDefaultHue{265};
    constexpr color::HueInteger kPlayerPurpleMaxHue{280};

    constexpr color::Saturation kNormalPlayerColorSaturation{1.0f};
    constexpr color::Saturation kImmunePlayerColorSaturation{0.5f};

}
