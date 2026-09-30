#pragma once

#include <utility>

#include <GameClient/Panorama/PanoramaLabel.h>
#include <Utils/StringBuilder.h>
#include <CS2/Panorama/CUIPanel.h>
#include <Features/Visuals/PlayerInfoInWorld/PlayerInfoInWorldConfigVariables.h>
#include <Features/Visuals/PlayerInfoInWorld/PlayerInfoPanelCacheEntry.h>
#include <GameClient/Panorama/PanoramaUiPanel.h>

template <typename HookContext>
class PlayerActiveWeaponAmmoPanel {
public:
    PlayerActiveWeaponAmmoPanel(HookContext& hookContext, cs2::CUIPanel* uiPanel, PlayerInfoPanelCacheEntry& cache) noexcept
        : hookContext{hookContext}
        , uiPanel{uiPanel}
        , cache{cache}
    {
    }

    void update(auto&& playerPawn) const noexcept
    {
        if (!shouldShowOn(playerPawn)) {
            panel().setVisible(false);
            return;
        }

        panel().setVisible(true);
        const auto ammo = playerPawn.getActiveWeapon().clipAmmo().valueOr(-1);
        if (cache.activeWeaponAmmo(ammo))
            panel().children()[0].clientPanel().template as<PanoramaLabel>().setText(StringBuilderStorage<10>{}.builder().put(ammo).cstring());
    }

private:
    [[nodiscard]] bool shouldShowOn(auto&& playerPawn) const noexcept
    {
        return GET_CONFIG_VAR(player_info_vars::ActiveWeaponAmmoEnabled) && playerPawn.getActiveWeapon().clipAmmo().greaterThan(-1).valueOr(true);
    }

    [[nodiscard]] decltype(auto) panel() const noexcept
    {
        return hookContext.template make<PanoramaUiPanel>(uiPanel);
    }

    HookContext& hookContext;
    cs2::CUIPanel* uiPanel;
    PlayerInfoPanelCacheEntry& cache;
};
