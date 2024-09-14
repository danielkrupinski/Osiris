#pragma once

#include <utility>

#include "OutlineGlowContext.h"

template <typename Context>
class OutlineGlowUnloadHandler {
public:
    template <typename... Args>
    OutlineGlowUnloadHandler(Args&&... args) noexcept
        : context{std::forward<Args>(args)...}
    {
    }

    void handleUnload() const noexcept
    {
        context.clearGlowSceneObjects();
    }

private:
    Context context;
};

template <typename HookContext>
OutlineGlowUnloadHandler(HookContext&) -> OutlineGlowUnloadHandler<OutlineGlowContext<HookContext>>;
