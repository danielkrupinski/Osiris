#pragma once

#include <cstdint>

#include <CS2/Classes/SceneObjectUpdaterHandle_t.h>
#include "C_BaseModelEntity.h"

namespace cs2
{

struct C_CSWeaponBase : C_BaseModelEntity {
    using m_iClip1 = std::int32_t;
    using sceneObjectUpdaterHandle = SceneObjectUpdaterHandle_t*;
};

}
