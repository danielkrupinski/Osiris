#pragma once

#include "ActiveWeaponIcon/PlayerActiveWeaponIconPanel.h"
#include "BombIcon/PlayerBombIconPanel.h"

template <typename HookContext>
class PlayerWeaponIconPanelContext {
public:
    PlayerWeaponIconPanelContext(HookContext& hookContext, cs2::CUIPanel* panel) noexcept
        : _hookContext{hookContext}
        , _panel{panel}
    {
    }

    [[nodiscard]] decltype(auto) activeWeaponIconPanel() const noexcept
    {
        return _hookContext.template make<PlayerActiveWeaponIconPanel<HookContext>>(panel().children()[0]);
    }

    [[nodiscard]] decltype(auto) bombIconPanel() const noexcept
    {
        return _hookContext.template make<PlayerBombIconPanel<HookContext>>(panel().children()[1]);
    }

private:
    [[nodiscard]] decltype(auto) panel() const noexcept
    {
        return _hookContext.template make<PanoramaUiPanel>(_panel);
    }

    HookContext& _hookContext;
    cs2::CUIPanel* _panel;
};

template <typename HookContext, typename Context = PlayerWeaponIconPanelContext<HookContext>>
class PlayerWeaponIconPanel {
public:
    template <typename... Args>
    explicit PlayerWeaponIconPanel(Args&&... args) noexcept
        : context{std::forward<Args>(args)...}
    {
    }

    void update(auto&& playerPawn) const noexcept
    {
        const auto bombIconPanelVisibility = context.bombIconPanel().update(playerPawn);
        context.activeWeaponIconPanel().update(playerPawn, bombIconPanelVisibility);
    }

private:
    Context context;
};
