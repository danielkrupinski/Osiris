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
        const auto shouldShowPlantingColor = context.shouldShowPlantingColor(playerPawn);
        context.panel().children()[0].setVisible(!shouldShowPlantingColor);
        context.panel().children()[1].setVisible(shouldShowPlantingColor);
    }

private:
    Context context;
};
