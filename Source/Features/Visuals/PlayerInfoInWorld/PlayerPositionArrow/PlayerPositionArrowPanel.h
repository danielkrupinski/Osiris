#pragma once

#include <utility>

#include <CS2/Constants/ColorConstants.h>
#include <CS2/Panorama/CUIPanel.h>
#include <Features/Visuals/PlayerInfoInWorld/PlayerInfoPanelCacheEntry.h>
#include <GameClient/Entities/TeamNumber.h>
#include <GameClient/Panorama/PanoramaUiPanel.h>

#include "PlayerPositionArrowColorType.h"

template <typename HookContext>
class PlayerPositionArrowPanel {
public:
    explicit PlayerPositionArrowPanel(HookContext& hookContext, cs2::CUIPanel* uiPanel, PlayerInfoPanelCacheEntry& cache) noexcept
        : hookContext{hookContext}
        , uiPanel{uiPanel}
        , cache{cache}
    {
    }

    void update(auto&& playerPawn) const noexcept
    {
        if (!GET_CONFIG_VAR(player_info_vars::PlayerPositionArrowEnabled)) {
            panel().setVisible(false);
            return;
        }

        panel().setVisible(true);

        const auto arrowColor = getArrowColor(playerPawn);
        if (cache.playerPositionArrowColor(arrowColor))
            panel().setWashColor(arrowColor);
    }

private:
    [[nodiscard]] cs2::Color getArrowColor(auto&& playerPawn) const noexcept
    {
        if (GET_CONFIG_VAR(player_info_vars::PlayerPositionArrowColorMode) == PlayerPositionArrowColorType::PlayerOrTeamColor) {
            const auto playerColor = getPlayerColor(playerPawn.playerController().playerColorIndex());
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

    [[nodiscard]] std::optional<cs2::Color> getPlayerColor(auto playerColorIndex) const noexcept
    {
        if (!playerColorIndex.hasValue())
            return {};

        switch (playerColorIndex.value()) {
        using enum cs2::PlayerColorIndex;
        case Blue: return cs2::kColorPlayerBlue;
        case Green: return cs2::kColorPlayerGreen;
        case Yellow: return cs2::kColorPlayerYellow;
        case Orange: return cs2::kColorPlayerOrange;
        case Purple: return cs2::kColorPlayerPurple;
        default: return {};
        }
    }

    [[nodiscard]] decltype(auto) panel() const noexcept
    {
        return hookContext.template make<PanoramaUiPanel>(uiPanel);
    }

    HookContext& hookContext;
    cs2::CUIPanel* uiPanel;
    PlayerInfoPanelCacheEntry& cache;
};
