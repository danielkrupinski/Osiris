#pragma once

#include <Common/Visibility.h>
#include "PostRoundTimerContext.h"

template <typename Context>
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

private:
    Context context;
};

template <typename HookContext>
PostRoundTimer(HookContext&) -> PostRoundTimer<PostRoundTimerContext<HookContext>>;
