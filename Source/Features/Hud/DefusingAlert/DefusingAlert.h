#pragma once

#include <utility>

#include "DefusingAlertConfigVariables.h"
#include "DefusingAlertContext.h"

template <typename HookContext, typename Context = DefusingAlertContext<HookContext>>
class DefusingAlert {
public:
    template <typename... Args>
    DefusingAlert(Args&&... args) noexcept
        : context{std::forward<Args>(args)...}
    {
    }

    void run() const noexcept
    {
        if (!shouldRun())
            return;

        if (shouldShowDefuseAlert())
            context.defusingAlertPanel().showAndUpdate();
        else
            context.defusingAlertPanel().hide();
    }

    void onDisable() const noexcept
    {
        context.defusingAlertPanel().hide();
    }

    void onUnload() const noexcept
    {
        context.uiEngine().deletePanelByHandle(context.state().defusingAlertContainerPanelHandle);
    }

private:
    [[nodiscard]] bool shouldRun() const noexcept
    {
        return context.config().template getVariable<DefusingAlertEnabled>();
    }

    [[nodiscard]] bool shouldShowDefuseAlert() const noexcept
    {
        return context.hasC4BeingDefused();
    }

    Context context;
};
