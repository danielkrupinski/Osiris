#pragma once

#include <OutlineGlow/GlowSceneObjects.h>
#include "PlayerOutlineGlowCondition.h"

template <typename HookContext>
class PlayerOutlineGlowContext {
public:
    explicit PlayerOutlineGlowContext(HookContext& hookContext) noexcept
        : hookContext{hookContext}
    {
    }

    [[nodiscard]] auto& state() const noexcept
    {
        return hookContext.featuresStates().visualFeaturesStates.playerOutlineGlowState;
    }

    [[nodiscard]] decltype(auto) condition() const noexcept
    {
        return hookContext.template make<PlayerOutlineGlowCondition>();
    }

    [[nodiscard]] decltype(auto) getGlowSceneObjectFor(auto&& baseEntity) const noexcept
    {
        return hookContext.template make<GlowSceneObjects>().getGlowSceneObject(baseEntity.handle());
    }

    [[nodiscard]] auto& viewRenderHook() const noexcept
    {
        return hookContext.hooks().viewRenderHook;
    }

    void clearGlowSceneObjects() const noexcept
    {
        hookContext.template make<GlowSceneObjects>().clearObjects();
    }

    [[nodiscard]] bool isLocalPlayerAlive() const noexcept
    {
        return hookContext.localPlayerController2().playerPawn().isAlive().value_or(true);
    }

private:
    HookContext& hookContext;
};
