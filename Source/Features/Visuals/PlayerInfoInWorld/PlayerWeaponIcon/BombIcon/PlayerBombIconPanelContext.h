#pragma once

#include <CS2/Panorama/CUIPanel.h>
#include <GameClasses/PanoramaUiPanel.h>

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
        if (state().showBombCarrierIcon)
            return playerPawn.isCarryingC4();
        if (state().showBombPlantingIcon)
            return playerPawn.carriedC4().isBeingPlanted().valueOr(false);
        return false;
    }

    [[nodiscard]] bool shouldShowPlantingColor(auto&& playerPawn) const noexcept
    {
        return state().showBombPlantingIcon && playerPawn.carriedC4().isBeingPlanted().valueOr(false);
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
