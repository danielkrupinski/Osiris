#pragma once

#include <CS2/Panorama/CUIPanel.h>
#include <GameClient/Panorama/PanoramaUiPanel.h>

template <typename HookContext>
class PlayerBombIconPanelContext {
public:
    PlayerBombIconPanelContext(HookContext& hookContext, cs2::CUIPanel* panel) noexcept
        : hookContext{hookContext}
        , _panel{panel}
    {
    }

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

    [[nodiscard]] auto& state() const noexcept
    {
        return hookContext.featuresStates().visualFeaturesStates.playerInfoInWorldState;
    }

    [[nodiscard]] decltype(auto) panel() const noexcept
    {
        return hookContext.template make<PanoramaUiPanel>(_panel);
    }

private:
    HookContext& hookContext;
    cs2::CUIPanel* _panel;
};
