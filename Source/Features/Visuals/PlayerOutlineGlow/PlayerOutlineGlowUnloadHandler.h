#pragma once

#include <utility>

#include "PlayerOutlineGlow.h"

template <typename Context>
class PlayerOutlineGlowUnloadHandler {
public:
    template <typename... Args>
    PlayerOutlineGlowUnloadHandler(Args&&... args) noexcept
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
PlayerOutlineGlowUnloadHandler(HookContext&) -> PlayerOutlineGlowUnloadHandler<PlayerOutlineGlowContext<HookContext>>;
