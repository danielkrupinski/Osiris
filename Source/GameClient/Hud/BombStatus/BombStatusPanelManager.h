#pragma once

#include <utility>

#include <Common/Visibility.h>
#include <Features/Hud/BombTimer/BombTimer.h>
#include <Features/Hud/PostRoundTimer/PostRoundTimer.h>

#include "BombStatusPanel.h"

template <typename HookContext>
class BombStatusPanelManager {
public:
    explicit BombStatusPanelManager(HookContext& hookContext) noexcept
        : hookContext{hookContext}
    {
    }

    void run() const
    {
        if (updateFeaturesReplacingBombStatusPanel() == Visibility::Visible)
            bombStatusPanel().hide();
        else
            bombStatusPanel().restore();
    }

private:
    [[nodiscard]] Visibility updateFeaturesReplacingBombStatusPanel() const
    {
        if (postRoundTimer().update() == Visibility::Visible) {
            bombTimer().forceHide();
            return Visibility::Visible;
        }
        return bombTimer().update();
    }

    [[nodiscard]] decltype(auto) bombStatusPanel() const
    {
        return hookContext.template make<BombStatusPanel>();
    }

    [[nodiscard]] decltype(auto) postRoundTimer() const
    {
        return hookContext.template make<PostRoundTimer>();
    }

    [[nodiscard]] decltype(auto) bombTimer() const
    {
        return hookContext.template make<BombTimer>();
    }

    HookContext& hookContext;
};
