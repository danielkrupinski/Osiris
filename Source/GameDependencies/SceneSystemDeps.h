#pragma once

#include <CS2/Classes/CSceneSystem.h>

struct SceneSystemDeps {
    SceneSystemDeps() = default;

    template <typename SceneSystemPatterns>
    explicit SceneSystemDeps(const SceneSystemPatterns& sceneSystemPatterns) noexcept
        : sceneSystem{sceneSystemPatterns.sceneSystem()}
        , deleteSceneObject{sceneSystemPatterns.deleteSceneObject()}
    {
    }

    cs2::CSceneSystem* sceneSystem;
    cs2::CSceneSystem::DeleteSceneObject* deleteSceneObject;
};
