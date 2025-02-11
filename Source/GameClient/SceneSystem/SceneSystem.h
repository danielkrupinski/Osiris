#pragma once

#include <MemoryPatterns/PatternTypes/SceneSystemPatternTypes.h>

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

    void allocateAttributeList(auto&& sceneObject) const noexcept
    {
        if (sceneObject && sceneSystem() && allocateAttributeListFn()) {
            allocateAttributeListFn()(sceneSystem(), sceneObject);
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

    [[nodiscard]] auto allocateAttributeListFn() const noexcept
    {
        return hookContext.sceneSystemPatternSearchResults().template get<AllocateAttributeListFunctionPointer>();
    }

    HookContext& hookContext;
};
