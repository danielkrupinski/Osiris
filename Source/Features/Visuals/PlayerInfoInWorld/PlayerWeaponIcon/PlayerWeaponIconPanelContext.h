#pragma once

#include <CS2/Panorama/CUIPanel.h>
#include <Features/Visuals/PlayerInfoInWorld/PlayerInfoPanelCacheEntry.h>
#include <GameClient/Panorama/PanoramaUiPanel.h>

#include "ActiveWeaponIcon/PlayerActiveWeaponIconPanel.h"
#include "BombIcon/PlayerBombIconPanel.h"

template <typename HookContext>
class PlayerWeaponIconPanelContext {
public:
    PlayerWeaponIconPanelContext(HookContext& hookContext, cs2::CUIPanel* panel, PlayerInfoPanelCacheEntry&) noexcept
        : _hookContext{hookContext}
        , _panel{panel}
    {
    }

    [[nodiscard]] decltype(auto) activeWeaponIconPanel() const noexcept
    {
        return _hookContext.template make<PlayerActiveWeaponIconPanel>(panel().children()[0]);
    }

    [[nodiscard]] decltype(auto) bombIconPanel() const noexcept
    {
        return _hookContext.template make<PlayerBombIconPanel>(panel().children()[1]);
    }

private:
    [[nodiscard]] decltype(auto) panel() const noexcept
    {
        return _hookContext.template make<PanoramaUiPanel>(_panel);
    }

    HookContext& _hookContext;
    cs2::CUIPanel* _panel;
};
