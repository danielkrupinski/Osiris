#pragma once

#include <cstdint>

#include "CSceneObject.h"

#include <Platform/Macros/IsPlatform.h>

namespace cs2
{

enum SceneObjectFlags {
    SceneObjectFlag_IsDeleted = 0x20
};

struct CGlowHelperSceneObject : CSceneObject {
    using entity = C_BaseEntity*;
    using flags = std::uint8_t;
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
