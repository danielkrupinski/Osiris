#pragma once

#include <CS2/Panorama/CUIPanel.h>
#include <GameClient/Panorama/PanoramaUiPanel.h>

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

    [[nodiscard]] decltype(auto) panoramaSymbols() const noexcept
    {
        return _hookContext.panoramaSymbols();
    }

    [[nodiscard]] decltype(auto) gameRules() const noexcept
    {
        return _hookContext.gameRules();
    }

    [[nodiscard]] decltype(auto) globalVars() const noexcept
    {
        return _hookContext.globalVars();
    }

private:
    HookContext& _hookContext;
    cs2::CUIPanel* _panel;
};
