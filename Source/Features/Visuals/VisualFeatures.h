#pragma once

#include <FeatureHelpers/FeatureHelpers.h>
#include "PlayerInformationThroughWalls/PlayerInformationThroughWalls.h"
#include "OutlineGlow/DefuseKitOutlineGlow/DefuseKitOutlineGlowToggle.h"
#include "OutlineGlow/GrenadeProjectileOutlineGlow/GrenadeProjectileOutlineGlowToggle.h"
#include "OutlineGlow/PlayerOutlineGlow/PlayerOutlineGlowToggle.h"
#include "OutlineGlow/WeaponOutlineGlow/WeaponOutlineGlowToggle.h"
#include "OutlineGlow/OutlineGlowToggle.h"
#include "VisualFeaturesStates.h"

class LoopModeGameHook;

struct VisualFeatures {
    [[nodiscard]] PlayerInformationThroughWallsToggle playerInformationThroughWalls() const noexcept
    {
        return PlayerInformationThroughWallsToggle{states.playerInformationThroughWallsState, hookDependencies, viewRenderHook};
    }

    [[nodiscard]] PlayerPositionToggle playerPositionToggle() const noexcept
    {
        return PlayerPositionToggle{states.playerInformationThroughWallsState};
    }

    [[nodiscard]] PlayerPositionArrowColorToggle playerPositionArrowColorToggle() const noexcept
    {
        return PlayerPositionArrowColorToggle{states.playerInformationThroughWallsState.playerPositionArrowColor};
    }

    [[nodiscard]] PlayerHealthToggle playerHealthToggle() const noexcept
    {
        return PlayerHealthToggle{states.playerInformationThroughWallsState};
    }

    [[nodiscard]] PlayerHealthTextColorToggle playerHealthTextColorToggle() const noexcept
    {
        return PlayerHealthTextColorToggle{states.playerInformationThroughWallsState.playerHealthTextColor};
    }

    [[nodiscard]] PlayerActiveWeaponToggle playerActiveWeaponToggle() const noexcept
    {
        return PlayerActiveWeaponToggle{states.playerInformationThroughWallsState};
    }

    [[nodiscard]] PlayerActiveWeaponAmmoToggle playerActiveWeaponAmmoToggle() const noexcept
    {
        return PlayerActiveWeaponAmmoToggle{states.playerInformationThroughWallsState};
    }

    [[nodiscard]] PlayerDefuseIconToggle playerDefuseIconToggle() const noexcept
    {
        return PlayerDefuseIconToggle{states.playerInformationThroughWallsState.playerStateIconsToShow};
    }

    [[nodiscard]] HostagePickupIconToggle hostagePickupIconToggle() const noexcept
    {
        return HostagePickupIconToggle{states.playerInformationThroughWallsState.playerStateIconsToShow};
    }

    [[nodiscard]] HostageRescueIconToggle hostageRescueIconToggle() const noexcept
    {
        return HostageRescueIconToggle{states.playerInformationThroughWallsState.playerStateIconsToShow};
    }

    [[nodiscard]] BlindedIconToggle blindedIconToggle() const noexcept
    {
        return BlindedIconToggle{states.playerInformationThroughWallsState.playerStateIconsToShow};
    }

    [[nodiscard]] decltype(auto) playerOutlineGlowToggle() const noexcept
    {
        return hookDependencies.make<PlayerOutlineGlowToggle>();
    }

    [[nodiscard]] decltype(auto) outlineGlowToggle() const noexcept
    {
        return hookDependencies.make<OutlineGlowToggle>();
    }

    [[nodiscard]] decltype(auto) weaponOutlineGlowToggle() const noexcept
    {
        return hookDependencies.make<WeaponOutlineGlowToggle>();
    }

    [[nodiscard]] decltype(auto) defuseKitOutlineGlowToggle() const noexcept
    {
        return hookDependencies.make<DefuseKitOutlineGlowToggle>();
    }

    [[nodiscard]] decltype(auto) grenadeProjectileOutlineGlowToggle() const noexcept
    {
        return hookDependencies.make<GrenadeProjectileOutlineGlowToggle>();
    }

    HookDependencies& hookDependencies;
    VisualFeaturesStates& states;
    ViewRenderHook& viewRenderHook;
};
