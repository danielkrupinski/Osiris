#pragma once

#include <utility>
#include <Common/Visibility.h>
#include "PostRoundTimerContext.h"

template <typename HookContext, typename Context = PostRoundTimerContext<HookContext>>
class PostRoundTimer {
public:
    template <typename... Args>
    PostRoundTimer(Args&&... args) noexcept
        : context{std::forward<Args>(args)...}
    {
    }

    [[nodiscard]] Visibility update() const noexcept
    {
        using enum Visibility;

        auto&& condition = context.condition();
        if (!condition.shouldRun())
            return Hidden;

        if (condition.shouldShowPostRoundTimer()) {
            context.postRoundTimerPanel().showAndUpdate();
            return Visible;
        } else {
            context.postRoundTimerPanel().hide();
            return Hidden;
        }
    }

    void onDisable() const noexcept
    {
        context.postRoundTimerPanel().hide();
    }

private:
    Context context;
};
