#pragma once

#include <utility>

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
        decltype(auto) condition{context.defusingAlertCondition()};
        if (!condition.shouldRun())
            return;

        if (condition.shouldShowDefuseAlert())
            context.defusingAlertPanel().showAndUpdate();
        else
            context.defusingAlertPanel().hide();
    }

    void onDisable() const noexcept
    {
        context.defusingAlertPanel().hide();
    }

private:
    Context context;
};
