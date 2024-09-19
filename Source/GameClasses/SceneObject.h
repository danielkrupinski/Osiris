#pragma once

#include <CS2/Classes/CSceneObject.h>

template <typename HookContext>
class SceneObject {
public:
    SceneObject(HookContext& hookContext, cs2::CSceneObject* sceneObject) noexcept
        : hookContext{hookContext}
        , sceneObject{sceneObject}
    {
    }

    [[nodiscard]] explicit operator bool() const noexcept
    {
        return sceneObject != nullptr;
    }

    [[nodiscard]] operator cs2::CSceneObject*() const noexcept
    {
        return sceneObject;
    }

    [[nodiscard]] auto isDeleted() const noexcept
    {
        return (hookContext.gameDependencies().sceneObjectDeps.offsetToFlags.of(sceneObject).toOptional() & cs2::SceneObjectFlag_IsDeleted).notEqual(0);
    }

    [[nodiscard]] auto objectClass() const noexcept
    {
        return hookContext.gameDependencies().sceneObjectDeps.offsetToClass.of(sceneObject).toOptional();
    }

private:
    HookContext& hookContext;
    cs2::CSceneObject* sceneObject;
};
