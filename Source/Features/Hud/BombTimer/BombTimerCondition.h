#pragma once

#include "BombTimerConfigVariables.h"

template <typename Context>
struct BombTimerCondition {
    explicit BombTimerCondition(Context context) noexcept
        : context{context}
    {
    }

    [[nodiscard]] bool shouldRun() const noexcept
    {
        return context.config().template getVariable<BombTimerEnabled>();
    }

    [[nodiscard]] bool shouldShowBombTimer() const noexcept
    {
        return context.bombPlantedPanel().isVisible().valueOr(true) && context.hasTickingC4();
    }

private:
    Context context;
};
