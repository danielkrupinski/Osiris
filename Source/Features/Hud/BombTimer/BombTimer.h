#pragma once

#include <utility>

#include <Common/Visibility.h>
#include "BombTimerContext.h"

template <typename Context>
class BombTimer {
public:
    template <typename... Args>
    explicit BombTimer(Args&&... args) noexcept
        : context{std::forward<Args>(args)...}
    {
    }

    [[nodiscard]] Visibility update() const noexcept
    {
        decltype(auto) condition{context.bombTimerCondition()};
        if (!condition.shouldRun())
            return Visibility::Hidden;

        if (condition.shouldShowBombTimer()) {
            context.bombTimerPanel().showAndUpdate();
            return Visibility::Visible;
        } else {
            context.bombTimerPanel().hide();
            return Visibility::Hidden;
        }
    }

    void forceHide() const noexcept
    {
        if (context.bombTimerCondition().shouldRun())
            context.bombTimerPanel().hide();
    }

private:
    Context context;
};

template <typename HookContext>
BombTimer(HookContext&) -> BombTimer<BombTimerContext<HookContext>>;
