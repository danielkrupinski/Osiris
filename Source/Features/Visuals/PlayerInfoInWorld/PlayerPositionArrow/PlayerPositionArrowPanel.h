#pragma once

#include <utility>
#include <FeatureHelpers/TeamNumber.h>

#include "PlayerPositionArrowColorType.h"
#include "PlayerPositionArrowPanelContext.h"
#include <CS2/Constants/ColorConstants.h>

template <typename HookContext, typename Context = PlayerPositionArrowPanelContext<HookContext>>
class PlayerPositionArrowPanel {
public:
    template <typename... Args>
    explicit PlayerPositionArrowPanel(Args&&... args) noexcept
        : context{std::forward<Args>(args)...}
    {
    }

    void update(auto&& playerPawn) const noexcept
    {
        if (!context.state().showPlayerPosition) {
            context.panel().setVisible(false);
            return;
        }

        context.panel().setVisible(true);

        const auto arrowColor = getArrowColor(playerPawn);
        if (context.cache().playerPositionArrowColor(arrowColor))
            context.panel().setWashColor(arrowColor);
    }

private:
    [[nodiscard]] cs2::Color getArrowColor(auto&& playerPawn) const noexcept
    {
        if (context.state().playerPositionArrowColor == PlayerPositionArrowColorType::PlayerOrTeamColor) {
            const auto playerColor = playerPawn.playerController().getPlayerColor();
            if (playerColor.has_value())
                return *playerColor;
        }

        switch (playerPawn.teamNumber()) {
            using enum TeamNumber;
            case TT: return cs2::kColorTeamTT;
            case CT: return cs2::kColorTeamCT;
            default: return cs2::kColorWhite;
        }
    }

    Context context;
};
