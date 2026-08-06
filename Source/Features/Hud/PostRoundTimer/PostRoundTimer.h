#pragma once

#include <utility>
#include <Common/Visibility.h>
#include "PostRoundTimerConfigVariables.h"
#include "PostRoundTimerContext.h"

template <typename HookContext, typename Context = PostRoundTimerContext<HookContext>>
class PostRoundTimer {
public:
    template <typename... Args>
    PostRoundTimer(Args&&... args) noexcept
        : context{std::forward<Args>(args)...}
    {
    }

    [[nodiscard]] Visibility update() const
    {
        using enum Visibility;

        if (!shouldRun())
            return Hidden;

        if (shouldShowPostRoundTimer()) {
            context.postRoundTimerPanel().showAndUpdate();
            return Visible;
        } else {
            context.postRoundTimerPanel().hide();
            return Hidden;
        }
    }

    void onDisable() const
    {
        context.postRoundTimerPanel().hide();
    }

    void onUnload() const
    {
        context.uiEngine().deletePanelByHandle(context.state().countdownContainerPanelHandle);
    }

private:
    [[nodiscard]] bool shouldRun() const
    {
        return context.config().template getVariable<PostRoundTimerEnabled>();
    }

    [[nodiscard]] bool shouldShowPostRoundTimer() const
    {
        return context.gameRules().hasScheduledRoundRestart() && !context.isGameRoundTimeVisible();
    }

    Context context;
};
