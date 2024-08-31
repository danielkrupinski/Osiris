#pragma once

#include "CUtlVector.h"
#include "SceneObjectUpdaterHandle_t.h"

namespace cs2
{

struct CRenderComponent {
    using sceneObjectUpdaters = cs2::CUtlVector<SceneObjectUpdaterHandle_t*>;
};

}
