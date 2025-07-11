#pragma once

#include <CS2/Classes/Color.h>
#include <CS2/Constants/ColorConstants.h>
#include <Utils/ColorUtils.h>

namespace model_glow_params
{
    constexpr cs2::Color kDefaultWeaponColor{cs2::kColorWhite};

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

    constexpr color::HueInteger kTeamTMinHue{25};
    constexpr color::HueInteger kTeamTDefaultHue{30};
    constexpr color::HueInteger kTeamTMaxHue{35};

    constexpr color::HueInteger kTeamCTMinHue{210};
    constexpr color::HueInteger kTeamCTDefaultHue{220};
    constexpr color::HueInteger kTeamCTMaxHue{230};

    constexpr color::HueInteger kLowHealthMinHue{0};
    constexpr color::HueInteger kLowHealthDefaultHue{0};
    constexpr color::HueInteger kLowHealthMaxHue{359};

    constexpr color::HueInteger kHighHealthMinHue{0};
    constexpr color::HueInteger kHighHealthDefaultHue{120};
    constexpr color::HueInteger kHighHealthMaxHue{359};

    constexpr color::HueInteger kAllyMinHue{0};
    constexpr color::HueInteger kAllyDefaultHue{120};
    constexpr color::HueInteger kAllyMaxHue{359};
;
    constexpr color::HueInteger kEnemyMinHue{0};
    constexpr color::HueInteger kEnemyDefaultHue{0};
    constexpr color::HueInteger kEnemyMaxHue{359};

    constexpr color::HueInteger kMolotovMinHue{20};
    constexpr color::HueInteger kMolotovDefaultHue{40};
    constexpr color::HueInteger kMolotovMaxHue{60};

    constexpr color::HueInteger kFlashbangMinHue{191};
    constexpr color::HueInteger kFlashbangDefaultHue{219};
    constexpr color::HueInteger kFlashbangMaxHue{250};

    constexpr color::HueInteger kHEGrenadeMinHue{300};
    constexpr color::HueInteger kHEGrenadeDefaultHue{359};
    constexpr color::HueInteger kHEGrenadeMaxHue{359};

    constexpr color::HueInteger kSmokeGrenadeMinHue{110};
    constexpr color::HueInteger kSmokeGrenadeDefaultHue{120};
    constexpr color::HueInteger kSmokeGrenadeMaxHue{140};

    constexpr color::HueInteger kDroppedBombMinHue{0};
    constexpr color::HueInteger kDroppedBombDefaultHue{60};
    constexpr color::HueInteger kDroppedBombMaxHue{359};

    constexpr color::HueInteger kTickingBombMinHue{0};
    constexpr color::HueInteger kTickingBombDefaultHue{0};
    constexpr color::HueInteger kTickingBombMaxHue{359};

    constexpr color::HueInteger kDefuseKitMinHue{0};
    constexpr color::HueInteger kDefuseKitDefaultHue{184};
    constexpr color::HueInteger kDefuseKitMaxHue{359};

    constexpr color::Saturation kNormalPlayerColorSaturation{1.0f};
    constexpr color::Saturation kImmunePlayerColorSaturation{0.5f};
}
