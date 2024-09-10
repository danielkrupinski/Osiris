#pragma once

template <typename HookContext>
class SceneSystem {
public:
    explicit SceneSystem(HookContext& hookContext) noexcept
        : hookContext{hookContext}
    {
    }

    void deleteSceneObject(auto&& sceneObject) const noexcept
    {
        if (sceneObject && !isSceneObjectDeleted(sceneObject).valueOr(false) && sceneSystem() && deleteSceneObjectFn())
            deleteSceneObjectFn()(sceneSystem(), sceneObject);
    }

private:
    [[nodiscard]] auto isSceneObjectDeleted(auto&& sceneObject) const noexcept
    {
        return (hookContext.gameDependencies().glowSceneObjectDeps.offsetToSceneObjectFlags.of(sceneObject).toOptional() & cs2::SceneObjectFlag_IsDeleted).notEqual(0);
    }

    [[nodiscard]] auto sceneSystem() const noexcept
    {
        return deps().sceneSystem;
    }

    [[nodiscard]] auto deleteSceneObjectFn() const noexcept
    {
        return deps().deleteSceneObject;
    }

    [[nodiscard]] const auto& deps() const noexcept
    {
        return hookContext.gameDependencies().sceneSystemDeps;
    }

    HookContext& hookContext;
};
