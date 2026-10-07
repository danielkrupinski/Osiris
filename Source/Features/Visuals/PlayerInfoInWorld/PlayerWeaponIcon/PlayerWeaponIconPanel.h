#pragma once

#include <utility>

#include <CS2/Panorama/CUIPanel.h>
#include <Features/Visuals/PlayerInfoInWorld/PlayerInfoPanelCacheEntry.h>
#include <GameClient/Panorama/PanoramaUiPanel.h>

#include "ActiveWeaponIcon/PlayerActiveWeaponIconPanel.h"
#include "BombIcon/PlayerBombIconPanel.h"

template <typename HookContext>
class PlayerWeaponIconPanel {
public:
    PlayerWeaponIconPanel(HookContext& hookContext, cs2::CUIPanel* uiPanel, PlayerInfoPanelCacheEntry&) noexcept
        : hookContext{hookContext}
        , uiPanel{uiPanel}
    {
    }

    void update(auto&& playerPawn) const noexcept
    {
        const auto bombIconPanelVisibility = bombIconPanel().update(playerPawn);
        activeWeaponIconPanel().update(playerPawn, bombIconPanelVisibility);
    }

private:
    [[nodiscard]] decltype(auto) activeWeaponIconPanel() const noexcept
    {
        return hookContext.template make<PlayerActiveWeaponIconPanel>(panel().children()[0]);
    }

    [[nodiscard]] decltype(auto) bombIconPanel() const noexcept
    {
        return hookContext.template make<PlayerBombIconPanel>(panel().children()[1]);
    }

    [[nodiscard]] decltype(auto) panel() const noexcept
    {
        return hookContext.template make<PanoramaUiPanel>(uiPanel);
    }

    HookContext& hookContext;
    cs2::CUIPanel* uiPanel;
};
