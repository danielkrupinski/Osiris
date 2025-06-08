#pragma once

#include "../../Config/ConfigVariable.h"

struct AimbotConfigVariables {
    ConfigVariable<bool> aimbotEnabled{"Aimbot.Enabled", false, "Enable or disable the aimbot."};
    ConfigVariable<float> aimbotFov{"Aimbot.Fov", 90.0f, "Aimbot field of view."};
    ConfigVariable<int> aimbotTargetBone{"Aimbot.TargetBone", 0, "Target bone for the aimbot (e.g., 0 for head, 1 for chest)."};
};
