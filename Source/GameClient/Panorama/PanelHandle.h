#pragma once

#include <CS2/Panorama/PanelHandle.h>
#include <GameClient/Panorama/PanoramaUiEngine.h>
#include <Utils/Lvalue.h>

#include <utility>

template <typename HookContext>
struct PanelHandle {
    PanelHandle(HookContext& hookContext, cs2::PanelHandle& handle) noexcept
        : hookContext{hookContext}
        , handle{handle}
    {
    }

    [[nodiscard]] decltype(auto) get() noexcept
    {
        return hookContext.template make<PanoramaUiEngine>().getPanelFromHandle(std::as_const(handle));
    }

    template <typename F>
    [[nodiscard]] decltype(auto) getOrInit(F&& f) noexcept
    {
        if (auto&& panel = get())
            return utils::lvalue<decltype(panel)>(panel);

        auto&& panel = std::forward<F>(f)();
        handle = panel.getHandle();
        return utils::lvalue<decltype(panel)>(panel);
    }

    [[nodiscard]] bool panelExists() noexcept
    {
        return handle.isValid() && static_cast<bool>(get());
    }

private:
    HookContext& hookContext;
    cs2::PanelHandle& handle;
};
