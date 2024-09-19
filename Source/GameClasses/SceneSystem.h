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
        if (sceneObject && sceneSystem() && deleteSceneObjectFn()) {
            deleteSceneObjectFn()(sceneSystem(), sceneObject);
        }
    }

private:
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
