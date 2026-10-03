#pragma once

#include <utility>

#include <Common/Visibility.h>

template <typename HookContext>
class PlayerBombIconPanel {
public:
    PlayerBombIconPanel(HookContext& hookContext, cs2::CUIPanel* uiPanel) noexcept
        : hookContext{hookContext}
        , uiPanel{uiPanel}
    {
    }

    [[nodiscard]] Visibility update(auto&& playerPawn) const noexcept
    {
        if (!shouldShowOnPlayer(playerPawn)) {
            panel().setVisible(false);
            return Visibility::Hidden;
        }

        panel().setVisible(true);
        const auto shouldShowPlantingColor_ = shouldShowPlantingColor(playerPawn);
        panel().children()[0].setVisible(!shouldShowPlantingColor_);
        panel().children()[1].setVisible(shouldShowPlantingColor_);
        return Visibility::Visible;
    }

private:
    [[nodiscard]] bool shouldShowOnPlayer(auto&& playerPawn) const noexcept
    {
        if (GET_CONFIG_VAR(player_info_vars::BombCarrierIconEnabled))
            return playerPawn.isCarryingC4();
        if (GET_CONFIG_VAR(player_info_vars::BombPlantIconEnabled))
            return playerPawn.carriedC4().isBeingPlanted().valueOr(false);
        return false;
    }

    [[nodiscard]] bool shouldShowPlantingColor(auto&& playerPawn) const noexcept
    {
        return GET_CONFIG_VAR(player_info_vars::BombPlantIconEnabled) && playerPawn.carriedC4().isBeingPlanted().valueOr(false);
    }

    [[nodiscard]] decltype(auto) panel() const noexcept
    {
        return hookContext.template make<PanoramaUiPanel>(uiPanel);
    }

    HookContext& hookContext;
    cs2::CUIPanel* uiPanel;
};
