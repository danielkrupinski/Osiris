#pragma once

template <typename Context>
struct PostRoundTimerCondition {
    explicit PostRoundTimerCondition(Context context) noexcept
        : context{context}
    {
    }

    [[nodiscard]] bool shouldRun() const noexcept
    {
        return context.config().template getVariable<PostRoundTimerEnabled>();
    }

    [[nodiscard]] bool shouldShowPostRoundTimer() const noexcept
    {
        return context.gameRules().hasScheduledRoundRestart() && !context.isGameRoundTimeVisible();
    }

private:
    Context context;
};
