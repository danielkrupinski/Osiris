#pragma once

template <typename Context>
class BombTimer {
public:
    explicit BombTimer(Context context) noexcept
        : context{context}
    {
    }

    void run() const noexcept
    {
        decltype(auto) condition{context.bombTimerCondition()};
        if (!condition.shouldRun())
            return;

        if (condition.shouldShowBombTimer()) {
            context.gameBombStatusPanel().hide();
            context.bombTimerPanel().showAndUpdate();
        } else {
            context.gameBombStatusPanel().restore();
            context.bombTimerPanel().hide();
        }
    }

private:
    Context context;
};
