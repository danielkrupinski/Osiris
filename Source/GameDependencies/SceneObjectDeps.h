#pragma once

#include <GameClasses/OffsetTypes/SceneObjectOffset.h>

struct SceneObjectDeps {
    SceneObjectDeps() = default;

    template <typename SceneObjectPatterns>
    explicit SceneObjectDeps(const SceneObjectPatterns& sceneObjectPatterns) noexcept
        : offsetToFlags{sceneObjectPatterns.offsetToSceneObjectFlags()}
        , offsetToClass{sceneObjectPatterns.offsetToSceneObjectClass()}
    {
    }

    OffsetToSceneObjectFlags offsetToFlags;
    OffsetToSceneObjectClass offsetToClass;
};
