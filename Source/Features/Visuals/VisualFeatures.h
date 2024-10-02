#pragma once

#include <FeatureHelpers/FeatureHelpers.h>
#include "PlayerInfoInWorld/PlayerInfoInWorld.h"
#include "OutlineGlow/DefuseKitOutlineGlow/DefuseKitOutlineGlowToggle.h"
#include "OutlineGlow/DroppedBombOutlineGlow/DroppedBombOutlineGlowToggle.h"
#include "OutlineGlow/GrenadeProjectileOutlineGlow/GrenadeProjectileOutlineGlowToggle.h"
#include "OutlineGlow/HostageOutlineGlow/HostageOutlineGlowToggle.h"
#include "OutlineGlow/PlayerOutlineGlow/PlayerOutlineGlowToggle.h"
#include "OutlineGlow/TickingBombOutlineGlow/TickingBombOutlineGlowToggle.h"
#include "OutlineGlow/WeaponOutlineGlow/WeaponOutlineGlowToggle.h"
#include "OutlineGlow/OutlineGlowToggle.h"
#include "VisualFeaturesStates.h"

class LoopModeGameHook;

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
        return PlayerInfoInWorldToggle{states.playerInfoInWorldState, hookContext, viewRenderHook};
    }

    [[nodiscard]] PlayerPositionToggle playerPositionToggle() const noexcept
    {
        return PlayerPositionToggle{states.playerInfoInWorldState};
    }

    [[nodiscard]] PlayerPositionArrowColorToggle playerPositionArrowColorToggle() const noexcept
    {
        return PlayerPositionArrowColorToggle{states.playerInfoInWorldState.playerPositionArrowColor};
    }

    [[nodiscard]] PlayerHealthToggle playerHealthToggle() const noexcept
    {
        return PlayerHealthToggle{states.playerInfoInWorldState};
    }

    [[nodiscard]] PlayerHealthTextColorToggle playerHealthTextColorToggle() const noexcept
    {
        return PlayerHealthTextColorToggle{states.playerInfoInWorldState.playerHealthTextColor};
    }

    [[nodiscard]] PlayerActiveWeaponToggle playerActiveWeaponToggle() const noexcept
    {
        return PlayerActiveWeaponToggle{states.playerInfoInWorldState};
    }

    [[nodiscard]] PlayerActiveWeaponAmmoToggle playerActiveWeaponAmmoToggle() const noexcept
    {
        return PlayerActiveWeaponAmmoToggle{states.playerInfoInWorldState};
    }

    [[nodiscard]] PlayerDefuseIconToggle playerDefuseIconToggle() const noexcept
    {
        return PlayerDefuseIconToggle{states.playerInfoInWorldState.playerStateIconsToShow};
    }

    [[nodiscard]] HostagePickupIconToggle hostagePickupIconToggle() const noexcept
    {
        return HostagePickupIconToggle{states.playerInfoInWorldState.playerStateIconsToShow};
    }

    [[nodiscard]] HostageRescueIconToggle hostageRescueIconToggle() const noexcept
    {
        return HostageRescueIconToggle{states.playerInfoInWorldState.playerStateIconsToShow};
    }

    [[nodiscard]] BlindedIconToggle blindedIconToggle() const noexcept
    {
        return BlindedIconToggle{states.playerInfoInWorldState.playerStateIconsToShow};
    }

    [[nodiscard]] BombIconToggle bombIconToggle() const noexcept
    {
        return BombIconToggle{states.playerInfoInWorldState.playerStateIconsToShow};
    }

    [[nodiscard]] decltype(auto) playerOutlineGlowToggle() const noexcept
    {
        return hookContext.template make<PlayerOutlineGlowToggle>();
    }

    [[nodiscard]] decltype(auto) outlineGlowToggle() const noexcept
    {
        return hookContext.template make<OutlineGlowToggle>();
    }

    [[nodiscard]] decltype(auto) weaponOutlineGlowToggle() const noexcept
    {
        return hookContext.template make<WeaponOutlineGlowToggle>();
    }

    [[nodiscard]] decltype(auto) defuseKitOutlineGlowToggle() const noexcept
    {
        return hookContext.template make<DefuseKitOutlineGlowToggle>();
    }

    [[nodiscard]] decltype(auto) grenadeProjectileOutlineGlowToggle() const noexcept
    {
        return hookContext.template make<GrenadeProjectileOutlineGlowToggle>();
    }

    [[nodiscard]] decltype(auto) droppedBombOutlineGlowToggle() const noexcept
    {
        return hookContext.template make<DroppedBombOutlineGlowToggle>();
    }

    [[nodiscard]] decltype(auto) tickingBombOutlineGlowToggle() const noexcept
    {
        return hookContext.template make<TickingBombOutlineGlowToggle>();
    }

    [[nodiscard]] decltype(auto) hostageOutlineGlowToggle() const noexcept
    {
        return hookContext.template make<HostageOutlineGlowToggle>();
    }

    HookContext& hookContext;
    VisualFeaturesStates& states;
    ViewRenderHook& viewRenderHook;
};
