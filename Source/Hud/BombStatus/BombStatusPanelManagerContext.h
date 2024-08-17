#pragma once

#include <Features/Hud/BombTimer/BombTimer.h>
#include <Features/Hud/PostRoundTimer/PostRoundTimer.h>

#include "BombStatusPanel.h"

template <typename HookContext>
struct BombStatusPanelManagerContext {
    explicit BombStatusPanelManagerContext(HookContext& hookContext) noexcept
        : hookContext{hookContext}
    {
    }

    [[nodiscard]] decltype(auto) bombStatusPanel() const noexcept
    {
        return hookContext.template make<BombStatusPanel>();
    }

    [[nodiscard]] decltype(auto) postRoundTimer() const noexcept
    {
        return hookContext.template make<PostRoundTimer>();
    }

    [[nodiscard]] decltype(auto) bombTimer() const noexcept
    {
        return hookContext.template make<BombTimer>();
    }

private:
    HookContext& hookContext;
};

