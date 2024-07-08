#pragma once

template <typename Context>
struct BombTimerPanel {
    void showAndUpdate() const noexcept
    {
        const auto tickingC4{context.tickingC4()};
        context.bombTimerContainerPanel().show();
        context.bombSiteIconPanel().setIcon(tickingC4.getBombSiteIconUrl());
        context.bombTimerTextPanel().setTimeToExplosion(tickingC4.getTimeToExplosion());
    }

    void hide() const noexcept
    {
        context.bombTimerContainerPanel().hide();
    }

    Context context;
};
