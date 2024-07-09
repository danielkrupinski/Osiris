#pragma once

template <typename Context>
struct BombTimerCondition {
    explicit BombTimerCondition(Context context) noexcept
        : context{context}
    {
    }

    [[nodiscard]] bool shouldRun() const noexcept
    {
        return context.state().enabled;
    }

    [[nodiscard]] bool shouldShowBombTimer() const noexcept
    {
        const auto plantedC4{context.plantedC4()};
        return plantedC4 && plantedC4->isTicking();
    }

private:
    Context context;
};
