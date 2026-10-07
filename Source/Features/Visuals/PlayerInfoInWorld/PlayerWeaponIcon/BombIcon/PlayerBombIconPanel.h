#pragma once

#include <utility>

#include <Common/Visibility.h>
#include <Features/Visuals/PlayerInfoInWorld/PlayerInfoInWorldConfigVariables.h>
#include <GameClient/Panorama/PanoramaUiPanel.h>
#include <HookContext/HookContextMacros.h>

template <typename HookContext>
class PlayerBombIconPanel {
public:
    PlayerBombIconPanel(HookContext& hookContext, cs2::CUIPanel* uiPanel) noexcept
        : hookContext{hookContext}
        , uiPanel{uiPanel}
    {
    }

    [[nodiscard]] Visibility update(auto&& playerPawn) const
    {
        auto&& panel = hookContext.template make<PanoramaUiPanel>(uiPanel);
        const auto visibleIcon = computeVisibleIcon(playerPawn);
        panel.setVisible(visibleIcon != Icon::None);

        if (visibleIcon == Icon::None)
            return Visibility::Hidden;

        auto&& childPanels = panel.children();
        childPanels[0].setVisible(visibleIcon == Icon::CarryingBomb);
        childPanels[1].setVisible(visibleIcon == Icon::PlantingBomb);
        return Visibility::Visible;
    }

private:
    enum class Icon {
        None,
        CarryingBomb,
        PlantingBomb
    };

    [[nodiscard]] Icon computeVisibleIcon(auto&& playerPawn) const
    {
        const auto carryingIconEnabled = GET_CONFIG_VAR(player_info_vars::BombCarrierIconEnabled);
        const auto plantingIconEnabled = GET_CONFIG_VAR(player_info_vars::BombPlantIconEnabled);

        if (!carryingIconEnabled && !plantingIconEnabled)
            return Icon::None;

        auto&& carriedC4 = playerPawn.carriedC4();
        if (plantingIconEnabled && carriedC4.isBeingPlanted().valueOr(false))
            return Icon::PlantingBomb;
        if (carryingIconEnabled && carriedC4)
            return Icon::CarryingBomb;
        return Icon::None;
    }

    HookContext& hookContext;
    cs2::CUIPanel* uiPanel;
};
