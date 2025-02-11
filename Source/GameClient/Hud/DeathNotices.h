#pragma once

#include <CS2/Panorama/CUIPanel.h>
#include <GameClient/Panorama/PanoramaUiPanel.h>
#include "DeathNotice.h"

template <typename HookContext>
struct DeathNotices {
    DeathNotices(HookContext& hookContext, cs2::CUIPanel* panel) noexcept
        : _hookContext{hookContext}
        , _panel{panel}
    {
    }

    template <typename F>
    void forEach(F&& f) noexcept
    {
        panel().children().forEach([&f](auto&& panel) { f(panel.template as<DeathNotice>()); });
    }

private:
    [[nodiscard]] decltype(auto) panel() noexcept
    {
        return _hookContext.template make<PanoramaUiPanel>(_panel);
    }

    HookContext& _hookContext;
    cs2::CUIPanel* _panel;
};
