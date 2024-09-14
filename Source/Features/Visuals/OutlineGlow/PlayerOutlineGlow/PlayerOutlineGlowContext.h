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

    [[nodiscard]] bool isLocalPlayerAlive() const noexcept
    {
        return hookContext.localPlayerController().playerPawn().isAlive().value_or(true);
    }

private:
    HookContext& hookContext;
};
