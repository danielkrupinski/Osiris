#pragma once

#include <Platform/Macros/IsPlatform.h>

#include "CSceneObject.h"
#include "Entities/C_BaseEntity.h"

namespace cs2
{

struct CGlowHelperSceneObject : CSceneObject {
    using entity = C_BaseEntity*;
    using attachedSceneObject = CSceneObject*;
};

#if IS_WIN64()

using ManageGlowSceneObject = void(
    CGlowHelperSceneObject** glowHelperSceneObject,
    CGlowHelperSceneObject** unused,
    CSceneObject* sceneObject,
    float color[4], float glowRangeMin, float glowRange, int glowType, float glowBackfaceMult);

#elif IS_LINUX()
          
using ManageGlowSceneObject = void(
    CGlowHelperSceneObject** glowHelperSceneObject,
    CGlowHelperSceneObject** unused,
    CSceneObject* sceneObject,
    int glowType,
    double colorGA, double colorRG, float glowRangeMin, float glowRange, float glowBackfaceMult);

#endif

}
