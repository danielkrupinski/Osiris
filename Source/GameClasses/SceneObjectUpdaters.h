#pragma once

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
