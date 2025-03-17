#pragma once

#include <CS2/Panorama/CUIPanel.h>
#include <Features/Visuals/PlayerInfoInWorld/PlayerInfoPanelCacheEntry.h>
#include <GameClient/Panorama/PanoramaUiPanel.h>

template <typename HookContext>
class PlayerActiveWeaponAmmoPanelContext {
public:
    PlayerActiveWeaponAmmoPanelContext(HookContext& hookContext, cs2::CUIPanel* panel, PlayerInfoPanelCacheEntry& cache) noexcept
        : _hookContext{hookContext}
        , _panel{panel}
        , _cache{cache}
    {
    }

    [[nodiscard]] auto& state() const noexcept
    {
        return _hookContext.featuresStates().visualFeaturesStates.playerInfoInWorldState;
    }

    [[nodiscard]] bool shouldShowOn(auto&& playerPawn) const noexcept
    {
        return _hookContext.config().template getVariable<PlayerInfoInWorldActiveWeaponAmmoEnabled>() && playerPawn.getActiveWeapon().clipAmmo().greaterThan(-1).valueOr(true);
    }

    [[nodiscard]] decltype(auto) panel() const noexcept
    {
        return _hookContext.template make<PanoramaUiPanel>(_panel);
    }

    [[nodiscard]] auto& cache() const noexcept
    {
        return _cache;
    }

private:
    HookContext& _hookContext;
    cs2::CUIPanel* _panel;
    PlayerInfoPanelCacheEntry& _cache;
};
