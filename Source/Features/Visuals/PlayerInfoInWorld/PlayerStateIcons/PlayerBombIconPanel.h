#pragma once

#include "PlayerBombIconPanelContext.h"
#include "PlayerStateIconsPanelParams.h"

template <typename HookContext, typename Context = PlayerBombIconPanelContext<HookContext>>
class PlayerBombIconPanel {
public:
    template <typename... Args>
    explicit PlayerBombIconPanel(Args&&... args) noexcept
        : context{std::forward<Args>(args)...}
    {
    }

    void update(auto&& playerPawn) const noexcept
    {
        if (!context.shouldShowOnPlayer(playerPawn)) {
            context.panel().setVisible(false);
            return;
        }

        context.panel().setVisible(true);
        context.panel().setWashColor(getColor(playerPawn));
    }

private:
    [[nodiscard]] auto getColor(auto&& playerPawn) const noexcept
    {
        using namespace player_state_icons_panel_params::bomb_icon_panel_params;
        return context.shouldShowPlantingColor(playerPawn) ? kWashColorPlantingC4 : kWashColorCarryingC4;
    }

    Context context;
};
