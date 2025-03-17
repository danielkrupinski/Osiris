#pragma once

#include "DefusingAlertConfigVariables.h"

template <typename Context>
struct DefusingAlertCondition {
    explicit DefusingAlertCondition(Context context) noexcept
        : context{context}
    {
    }

    [[nodiscard]] bool shouldRun() const noexcept
    {
        return context.config().template getVariable<DefusingAlertEnabled>();
    }

    [[nodiscard]] bool shouldShowDefuseAlert() const noexcept
    {
        return context.hasC4BeingDefused();
    }

private:
    Context context;
};
