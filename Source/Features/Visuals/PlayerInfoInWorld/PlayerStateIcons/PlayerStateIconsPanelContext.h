#pragma once

#include <GameClasses/PanoramaUiPanel.h>
#include "PlayerBombIconPanel.h"

template <typename HookContext>
class PlayerStateIconsPanelContext {
public:
    PlayerStateIconsPanelContext(HookContext& hookContext, cs2::CUIPanel* panel) noexcept
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

    [[nodiscard]] decltype(auto) bombIconPanel() const noexcept
    {
        return _hookContext.template make<PlayerBombIconPanel<HookContext>>(panel().children()[4]);
    }

private:
    HookContext& _hookContext;
    cs2::CUIPanel* _panel;
};
