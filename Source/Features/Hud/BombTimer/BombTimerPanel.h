#pragma once

template <typename Context>
struct BombTimerPanel {
    explicit BombTimerPanel(Context context) noexcept
        : context{context}
    {
    }

    void showAndUpdate() const noexcept
    {
        context.bombTimerContainerPanel().show();

        decltype(auto) tickingC4{context.tickingC4()};
        context.bombSiteIconPanel().setIcon(tickingC4.getBombSiteIconUrl());
        context.bombTimerTextPanel().setTimeToExplosion(tickingC4.getTimeToExplosion().valueOr(0.0f));
    }

    void hide() const noexcept
    {
        context.bombTimerContainerPanel().hide();
    }

private:
    Context context;
};
