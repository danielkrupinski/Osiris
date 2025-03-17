#pragma once

#include <Hooks/ViewRenderHook.h>
#include "ModelGlow/ModelGlowToggle.h"
#include "PlayerInfoInWorld/PlayerInfoInWorld.h"
#include "OutlineGlow/PlayerOutlineGlow/PlayerOutlineGlowToggle.h"
#include "VisualFeaturesStates.h"

template <typename HookContext>
struct VisualFeatures {
    VisualFeatures(HookContext& hookContext, VisualFeaturesStates& states, ViewRenderHook& viewRenderHook) noexcept
        : hookContext{hookContext}
        , states{states}
        , viewRenderHook{viewRenderHook}
    {
    }

    [[nodiscard]] auto playerInfoInWorld() const noexcept
    {
        return PlayerInfoInWorldToggle{hookContext};
    }

    [[nodiscard]] decltype(auto) playerOutlineGlowToggle() const noexcept
    {
        return hookContext.template make<PlayerOutlineGlowToggle>();
    }

    [[nodiscard]] decltype(auto) modelGlowToggle() const noexcept
    {
        return hookContext.template make<ModelGlowToggle>();
    }

    HookContext& hookContext;
    VisualFeaturesStates& states;
    ViewRenderHook& viewRenderHook;
};
