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

template <typename Context>
class PlayerOutlineGlow {
public:
    template <typename... Args>
    PlayerOutlineGlow(Args&&... args) noexcept
        : context{std::forward<Args>(args)...}
    {
    }

    void applyGlowToPlayer(auto&& playerPawn) const noexcept
    {
        auto&& condition = context.condition();
        if (!condition.shouldRun() || !condition.shouldGlowPlayer(playerPawn))
            return;

        applyGlow(playerPawn.baseEntity(), getColor(playerPawn));
    }

private:
    void applyGlow(auto&& baseEntity, cs2::Color color) const noexcept
    {
        context.getGlowSceneObjectFor(baseEntity).apply(baseEntity, color.setAlpha(102));
    }

    [[nodiscard]] cs2::Color getColor(auto&& playerPawn) const noexcept
    {
        switch (playerPawn.teamNumber()) {
        case TeamNumber::TT: return cs2::kColorTeamTT;
        case TeamNumber::CT: return cs2::kColorTeamCT;
        default: return cs2::kColorWhite;
        }
    }

    Context context;
};

template <typename HookContext>
PlayerOutlineGlow(HookContext&) -> PlayerOutlineGlow<PlayerOutlineGlowContext<HookContext>>;
