#pragma once

#include <CS2/Panorama/CUIPanel.h>
#include <Features/Visuals/PlayerInfoInWorld/PlayerInfoInWorldConfigVariables.h>
#include <Features/Visuals/PlayerInfoInWorld/PlayerInfoPanelCacheEntry.h>
#include <GameClient/Panorama/PanoramaUiPanel.h>

template <typename HookContext>
class PlayerActiveWeaponAmmoPanelContext {
public:
    PlayerActiveWeaponAmmoPanelContext(HookContext& hookContext, cs2::CUIPanel* panel, PlayerInfoPanelCacheEntry& cache) noexcept
        : hookContext{hookContext}
        , _panel{panel}
        , _cache{cache}
    {
    }

    [[nodiscard]] auto& state() const noexcept
    {
        return hookContext.featuresStates().visualFeaturesStates.playerInfoInWorldState;
    }

    [[nodiscard]] bool shouldShowOn(auto&& playerPawn) const noexcept
    {
        return GET_CONFIG_VAR(player_info_vars::ActiveWeaponAmmoEnabled) && playerPawn.getActiveWeapon().clipAmmo().greaterThan(-1).valueOr(true);
    }

    [[nodiscard]] decltype(auto) panel() const noexcept
    {
        return hookContext.template make<PanoramaUiPanel>(_panel);
    }

    [[nodiscard]] auto& cache() const noexcept
    {
        return _cache;
    }

private:
    HookContext& hookContext;
    cs2::CUIPanel* _panel;
    PlayerInfoPanelCacheEntry& _cache;
};
