#pragma once

#include <cstdint>

#include <CS2/Classes/SceneObjectUpdaterHandle_t.h>
#include <CS2/Classes/Entities/C_EconEntity.h>

namespace cs2
{

struct C_CSWeaponBase : C_EconEntity {
    using m_iClip1 = std::int32_t;
    using sceneObjectUpdaterHandle = SceneObjectUpdaterHandle_t*;
    using GetInaccuracy = float(C_CSWeaponBase* thisptr, float* movementInaccuracy, float* airSpeedInaccuracy);
    using GetSpread = float(C_CSWeaponBase* thisptr);
    using m_flThrowStrength = float;
    using m_bPinPulled = bool;
};

}
