#pragma once

#include <utility>

#include <Common/Visibility.h>
#include "BombTimerConfigVariables.h"
#include "BombTimerContext.h"

template <typename HookContext, typename Context = BombTimerContext<HookContext>>
class BombTimer {
public:
    template <typename... Args>
    explicit BombTimer(Args&&... args) noexcept
        : context{std::forward<Args>(args)...}
    {
    }

    [[nodiscard]] Visibility update() const
    {
        if (!shouldRun())
            return Visibility::Hidden;

        if (shouldShowBombTimer()) {
            context.bombTimerPanel().showAndUpdate();
            return Visibility::Visible;
        } else {
            context.bombTimerPanel().hide();
            return Visibility::Hidden;
        }
    }

    void forceHide() const
    {
        if (shouldRun())
            context.bombTimerPanel().hide();
    }

    void onDisable() const
    {
        context.bombTimerPanel().hide();
    }

    void onUnload() const
    {
        auto&& uiEngine = context.uiEngine();
        uiEngine.deletePanelByHandle(context.state().bombTimerPanelHandle);
        uiEngine.deletePanelByHandle(context.state().bombTimerContainerPanelHandle);
    }

private:
    [[nodiscard]] bool shouldRun() const
    {
        return context.config().template getVariable<BombTimerEnabled>();
    }

    [[nodiscard]] bool shouldShowBombTimer() const
    {
        return context.bombPlantedPanel().isVisible().valueOr(true) && context.hasTickingC4();
    }

    Context context;
};
