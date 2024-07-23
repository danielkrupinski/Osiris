#pragma once

#include <CS2/Classes/Panorama.h>

template <typename HookContext>
struct ClientPanel {
    ClientPanel(HookContext& hookContext, cs2::CPanel2D* panel) noexcept
        : hookContext{hookContext}
        , panel{panel}
    {
    }

    template <template <typename> typename T>
    [[nodiscard]] decltype(auto) as() const noexcept
    {
        return hookContext.template make<T>(panel);
    }

private:
    HookContext& hookContext;
    cs2::CPanel2D* panel;
};
