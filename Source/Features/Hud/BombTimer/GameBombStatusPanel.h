#pragma once

template <typename Context>
struct GameBombStatusPanel {
    explicit GameBombStatusPanel(Context context) noexcept
        : context{context}
    {
    }

    void hide() const noexcept
    {
        context.bombStatusPanel().setParent(context.invisiblePanel());
    }

    void restore() const noexcept
    {
        context.bombStatusPanel().setParent(context.scoreAndTimeAndBombPanel());
    }

private:
    Context context;
};
