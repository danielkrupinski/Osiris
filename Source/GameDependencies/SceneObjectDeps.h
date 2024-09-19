#pragma once

#include <GameClasses/OffsetTypes/SceneObjectOffset.h>

struct SceneObjectDeps {
    template <typename SceneObjectPatterns>
    explicit SceneObjectDeps(const SceneObjectPatterns& sceneObjectPatterns) noexcept
        : offsetToFlags{sceneObjectPatterns.offsetToSceneObjectFlags()}
        , offsetToClass{sceneObjectPatterns.offsetToSceneObjectClass()}
    {
    }

    OffsetToSceneObjectFlags offsetToFlags;
    OffsetToSceneObjectClass offsetToClass;
};
