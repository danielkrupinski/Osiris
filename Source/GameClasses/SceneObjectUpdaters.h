#pragma once

#include <cstddef>
#include <utility>

#include <CS2/Classes/CRenderComponent.h>
#include <CS2/Classes/SceneObjectUpdaterHandle_t.h>

#include "SceneObjectUpdater.h"

template <typename HookContext>
class SceneObjectUpdaters {
public:
    SceneObjectUpdaters(HookContext& hookContext, cs2::CRenderComponent::sceneObjectUpdaters* updaters) noexcept
        : hookContext{hookContext}
        , updaters{updaters}
    {
    }

    [[nodiscard]] decltype(auto) operator[](std::size_t index) const noexcept
    {
        return hookContext.template make<SceneObjectUpdater>(get(index));
    }

    template <typename F>
    void forEachSceneObject(F f) const noexcept
    {
        if (!updaters)
            return;

        for (int i = 0; i < updaters->size; ++i) {
            auto&& sceneObject = hookContext.template make<SceneObjectUpdater>(updaters->memory[i]).sceneObject();
            if (sceneObject)
                f(sceneObject);
        }
    }

private:
    [[nodiscard]] cs2::SceneObjectUpdaterHandle_t* get(std::size_t index) const noexcept
    {
        if (updaters && std::cmp_less(index, updaters->size))
            return updaters->memory[index];
        return nullptr;
    }
    
    HookContext& hookContext;
    cs2::CRenderComponent::sceneObjectUpdaters* updaters;
};
