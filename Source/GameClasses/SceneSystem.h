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
        return hookContext.sceneSystemPatternSearchResults().template get<SceneSystemPointer>();
    }

    [[nodiscard]] auto deleteSceneObjectFn() const noexcept
    {
        return hookContext.sceneSystemPatternSearchResults().template get<DeleteSceneObjectFunctionPointer>();
    }

    HookContext& hookContext;
};
