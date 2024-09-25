#pragma once

#include <CS2/Panorama/PanelHandle.h>
#include <Utils/Lvalue.h>

#include <utility>

template <typename HookContext>
struct PanelHandle {
    PanelHandle(HookContext& hookContext, cs2::PanelHandle& handle) noexcept
        : hookContext{hookContext}
        , handle{handle}
    {
    }

    template <typename F>
    [[nodiscard]] decltype(auto) getOrInit(F&& f) noexcept
    {
        if (auto&& panel = hookContext.template make<PanoramaUiEngine>().getPanelFromHandle(std::as_const(handle)))
            return utils::lvalue<decltype(panel)>(panel);

        auto&& panel = std::forward<F>(f)();
        handle = panel.getHandle();
        return utils::lvalue<decltype(panel)>(panel);
    }

private:
    HookContext& hookContext;
    cs2::PanelHandle& handle;
};
