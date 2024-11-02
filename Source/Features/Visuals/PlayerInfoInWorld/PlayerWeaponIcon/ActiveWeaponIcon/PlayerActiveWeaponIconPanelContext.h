#pragma once

#include <CS2/Panorama/CUIPanel.h>
#include <GameClasses/PanoramaUiPanel.h>

template <typename HookContext>
class PlayerActiveWeaponIconPanelContext {
public:
    PlayerActiveWeaponIconPanelContext(HookContext& hookContext, cs2::CUIPanel* panel) noexcept
        : _hookContext{hookContext}
        , _panel{panel}
    {
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
