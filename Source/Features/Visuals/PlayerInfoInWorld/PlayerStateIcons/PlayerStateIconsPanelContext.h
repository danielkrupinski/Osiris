#pragma once

#include <Features/Visuals/PlayerInfoInWorld/PlayerInfoPanelCacheEntry.h>
#include <GameClasses/PanoramaUiPanel.h>

template <typename HookContext>
class PlayerStateIconsPanelContext {
public:
    PlayerStateIconsPanelContext(HookContext& hookContext, cs2::CUIPanel* panel, PlayerInfoPanelCacheEntry&) noexcept
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
