#pragma once

#include <GameClasses/OffsetTypes/SceneObjectUpdaterOffset.h>

struct SceneObjectUpdaterDeps {
    SceneObjectUpdaterDeps() = default;

    template <typename SceneObjectUpdaterPatterns>
    explicit SceneObjectUpdaterDeps(const SceneObjectUpdaterPatterns& sceneObjectUpdaterPatterns) noexcept
        : offsetToSceneObject{sceneObjectUpdaterPatterns.offsetToSceneObject()}
    {
    }

    OffsetToSceneObject offsetToSceneObject;
};
