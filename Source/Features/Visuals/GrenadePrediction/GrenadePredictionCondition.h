#pragma once

#include "GrenadePredictionConfigVariables.h"

template <typename Context>
class GrenadePredictionCondition {
public:
    explicit GrenadePredictionCondition(auto& otherContext) noexcept
        : context{otherContext}
    {
    }

    [[nodiscard]] bool shouldRun() const noexcept
    {
        return context.config().template getVariable<grenade_prediction_vars::Enabled>();
    }

private:
    Context context;
};
