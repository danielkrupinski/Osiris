#pragma once

template <typename Context>
class BombTimer {
public:
    BombTimer(Context context) noexcept
        : context{context}
    {
    }

    void run() const noexcept
    {
        if (!context.state().enabled)
            return;

        if (const auto plantedC4{context.plantedC4()}; plantedC4 && plantedC4->isTicking()) {
            context.gameBombStatusPanel().hide();
            context.bombTimerPanel().showAndUpdate(*plantedC4);
        } else {
            context.gameBombStatusPanel().restore();
            context.bombTimerPanel().hide();
        }
    }

private:
    Context context;
};
