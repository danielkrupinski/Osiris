#pragma once

template <typename Context>
struct GameBombStatusPanel {
    void hide() const noexcept
    {
        context.bombStatusPanel().setParent(context.invisiblePanel());
    }

    void restore() const noexcept
    {
        context.bombStatusPanel().setParent(context.scoreAndTimeAndBombPanel());
    }

    Context context;
};
