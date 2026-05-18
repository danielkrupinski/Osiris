#pragma once

#include <utility>

#include <CS2/Panorama/CUIPanel.h>
#include <Features/Visuals/PlayerInfoInWorld/PlayerInfoInWorldConfigVariables.h>
#include <GameClient/Panorama/PanoramaLabel.h>
#include <GameClient/Panorama/PanoramaUiPanel.h>

#include "PlayerNamePanelParams.h"

template <typename HookContext>
class PlayerNamePanelContext {
public:
    PlayerNamePanelContext(HookContext& hookContext, cs2::CUIPanel* panel, auto&) noexcept
        : _hookContext{hookContext}
        , _panel{panel}
    {
    }

    [[nodiscard]] decltype(auto) config() const noexcept
    {
        return _hookContext.config();
    }

    [[nodiscard]] decltype(auto) panel() const noexcept
    {
        return _hookContext.template make<PanoramaUiPanel>(_panel);
    }

private:
    HookContext& _hookContext;
    cs2::CUIPanel* _panel;
};

template <typename HookContext, typename Context = PlayerNamePanelContext<HookContext>>
class PlayerNamePanel {
public:
    template <typename... Args>
    explicit PlayerNamePanel(Args&&... args) noexcept
        : context{std::forward<Args>(args)...}
    {
    }

    void update(auto&& playerPawn) const noexcept
    {
        if (!context.config().template getVariable<player_info_vars::PlayerNameEnabled>()) {
            context.panel().setVisible(false);
            return;
        }

        const auto playerName = playerPawn.playerController().getName();
        if (!playerName || !*playerName) {
            context.panel().setVisible(false);
            return;
        }

        context.panel().setVisible(true);
        context.panel().clientPanel().template as<PanoramaLabel>().setText(playerName);
    }

private:
    Context context;
};
