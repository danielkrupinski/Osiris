#pragma once

#include <CS2/Classes/Panorama.h>
#include <GameClasses/PanoramaUiPanel.h>

template <typename HookContext>
struct DeathNoticeContext {
    DeathNoticeContext(HookContext& hookContext, cs2::CUIPanel* panel) noexcept
        : _hookContext{hookContext}
        , _panel{panel}
    {
    }

    [[nodiscard]] decltype(auto) panel() const noexcept
    {
        return _hookContext.template make<PanoramaUiPanel>(_panel);
    }

    [[nodiscard]] decltype(auto) gameRules() const noexcept
    {
        return _hookContext.gameRules();
    }

    [[nodiscard]] decltype(auto) globalVars() const noexcept
    {
        return _hookContext.globalVars();
    }

    HookContext& _hookContext;
    cs2::CUIPanel* _panel;
};
