#pragma once

#include <utility>

#include "BombStatusPanelManagerContext.h"
#include <Common/Visibility.h>

template <typename HookContext, typename Context = BombStatusPanelManagerContext<HookContext>>
class BombStatusPanelManager {
public:
    template <typename... Args>
    BombStatusPanelManager(Args&&... args) noexcept
        : context{std::forward<Args>(args)...}
    {
    }

    void run() const noexcept
    {
        if (updateFeaturesReplacingBombStatusPanel() == Visibility::Visible)
            context.bombStatusPanel().hide();
        else
            context.bombStatusPanel().restore();
    }

private:
    [[nodiscard]] Visibility updateFeaturesReplacingBombStatusPanel() const noexcept
    {
        if (context.postRoundTimer().update() == Visibility::Visible) {
            context.bombTimer().forceHide();
            return Visibility::Visible;
        }
        return context.bombTimer().update();
    }

    Context context;
};
