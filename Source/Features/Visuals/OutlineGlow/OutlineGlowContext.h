#pragma once

#include <GameClasses/PlayerPawn.h>
#include <OutlineGlow/GlowSceneObjects.h>

#include "PlayerOutlineGlow/PlayerOutlineGlow.h"
#include "WeaponOutlineGlow/WeaponOutlineGlow.h"

template <typename HookContext>
class OutlineGlowContext {
public:
    explicit OutlineGlowContext(HookContext& hookContext) noexcept
        : hookContext{hookContext}
    {
    }

    [[nodiscard]] auto& state() const noexcept
    {
        return hookContext.featuresStates().visualFeaturesStates.outlineGlowState;
    }

    void applyGlowToPlayer(auto& entity) const noexcept
    {
        hookContext.template make<PlayerOutlineGlow>().applyGlowToPlayer(hookContext.template make<PlayerPawn>(static_cast<cs2::C_CSPlayerPawn*>(&entity)));
    }

    void applyGlowToWeapon(EntityTypeInfo entityTypeInfo, auto& entity) const noexcept
    {
        hookContext.template make<WeaponOutlineGlow>().applyGlowToWeapon(entityTypeInfo, hookContext.template make<BaseEntity>(static_cast<cs2::C_CSWeaponBase*>(&entity)));
    }

    [[nodiscard]] auto& viewRenderHook() const noexcept
    {
        return hookContext.hooks().viewRenderHook;
    }

    void clearGlowSceneObjects() const noexcept
    {
        hookContext.template make<GlowSceneObjects>().clearObjects();
    }

private:
    HookContext& hookContext;
};
