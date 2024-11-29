#pragma once

#include <utility>

#include "OutlineGlowContext.h"

template <typename HookContext, typename Context = OutlineGlowContext<HookContext>>
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
