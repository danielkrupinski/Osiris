#pragma once

#include <Config/HueVariableParams.h>
#include <CS2/Classes/Color.h>
#include <CS2/Constants/ColorConstants.h>
#include <Utils/ColorUtils.h>

namespace model_glow_params
{
    constexpr cs2::Color kFallbackColor{cs2::kColorWhite};

    constexpr HueVariableParams kPlayerBlueHue{.min = 191, .max = 240, .def = 215};
    constexpr HueVariableParams kPlayerGreenHue{.min = 110, .max = 140, .def = 125};
    constexpr HueVariableParams kPlayerYellowHue{.min = 47, .max = 60, .def = 53};
    constexpr HueVariableParams kPlayerOrangeHue{.min = 11, .max = 20, .def = 15};
    constexpr HueVariableParams kPlayerPurpleHue{.min = 250, .max = 280, .def = 265};

    constexpr HueVariableParams kTeamTHue{.min = 30, .max = 40, .def = 35};
    constexpr HueVariableParams kTeamCTHue{.min = 210, .max = 230, .def = 220};

    constexpr HueVariableParams kLowHealthHue{.min = 0, .max = 359, .def = 0};
    constexpr HueVariableParams kHighHealthHue{.min = 0, .max = 359, .def = 120};

    constexpr HueVariableParams kAllyHue{.min = 0, .max = 359, .def = 120};
    constexpr HueVariableParams kEnemyHue{.min = 0, .max = 359, .def = 0};

    constexpr HueVariableParams kMolotovHue{.min = 20, .max = 60, .def = 40};
    constexpr HueVariableParams kFlashbangHue{.min = 191, .max = 250, .def = 219};
    constexpr HueVariableParams kHEGrenadeHue{.min = 300, .max = 359, .def = 359};
    constexpr HueVariableParams kSmokeGrenadeHue{.min = 110, .max = 140, .def = 120};

    constexpr HueVariableParams kDroppedBombHue{.min = 0, .max = 359, .def = 60};
    constexpr HueVariableParams kTickingBombHue{.min = 0, .max = 359, .def = 0};
    constexpr HueVariableParams kDefuseKitHue{.min = 0, .max = 359, .def = 184};

    constexpr color::Saturation kNormalPlayerColorSaturation{1.0f};
    constexpr color::Saturation kImmunePlayerColorSaturation{0.5f};
}
