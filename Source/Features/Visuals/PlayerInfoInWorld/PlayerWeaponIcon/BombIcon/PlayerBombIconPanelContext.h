#pragma once

#include <CS2/Panorama/CUIPanel.h>
#include <GameClient/Panorama/PanoramaUiPanel.h>

template <typename HookContext>
class PlayerBombIconPanelContext {
public:
    PlayerBombIconPanelContext(HookContext& hookContext, cs2::CUIPanel* panel) noexcept
        : _hookContext{hookContext}
        , _panel{panel}
    {
    }

    [[nodiscard]] bool shouldShowOnPlayer(auto&& playerPawn) const noexcept
    {
        if (_hookContext.config().template getVariable<PlayerInfoInWorldBombCarrierIconEnabled>())
            return playerPawn.isCarryingC4();
        if (_hookContext.config().template getVariable<PlayerInfoInWorldBombPlantIconEnabled>())
            return playerPawn.carriedC4().isBeingPlanted().valueOr(false);
        return false;
    }

    [[nodiscard]] bool shouldShowPlantingColor(auto&& playerPawn) const noexcept
    {
        return _hookContext.config().template getVariable<PlayerInfoInWorldBombPlantIconEnabled>() && playerPawn.carriedC4().isBeingPlanted().valueOr(false);
    }

    [[nodiscard]] auto& state() const noexcept
    {
        return _hookContext.featuresStates().visualFeaturesStates.playerInfoInWorldState;
    }

    [[nodiscard]] decltype(auto) panel() const noexcept
    {
        return _hookContext.template make<PanoramaUiPanel>(_panel);
    }

private:
    HookContext& _hookContext;
    cs2::CUIPanel* _panel;
};
