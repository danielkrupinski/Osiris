#pragma once

#include <utility>

#include <Common/Visibility.h>
#include "BombStatusPanelContext.h"

template <typename HookContext, typename Context = BombStatusPanelContext<HookContext>>
struct BombStatusPanel {
    template <typename... Args>
    explicit BombStatusPanel(Args&&... args) noexcept
        : context{std::forward<Args>(args)...}
    {
    }

    void hide() noexcept
    {
        changeVisibility(Visibility::Hidden, [this] { context.bombStatusPanel().setParent(context.invisiblePanel()); });
    }

    void restore() noexcept
    {
        changeVisibility(Visibility::Visible, [this] { context.bombStatusPanel().setParent(context.scoreAndTimeAndBombPanel()); });
    }

private:
    void changeVisibility(Visibility targetVisibility, auto performVisibilityChange) noexcept
    {
        if (auto& visibility = context.visibility(); visibility != targetVisibility) {
            performVisibilityChange();
            visibility = targetVisibility;
        }
    }
    
    Context context;
};
