#pragma once

template <typename Context>
struct BombTimerPanel {
    void showAndUpdate(const auto bomb) const noexcept
    {
        context.bombTimerContainerPanel().show();
        context.bombSiteIconPanel().setIcon(bomb.getBombSiteIconUrl());
        context.bombTimerTextPanel().setTimeToExplosion(bomb.getTimeToExplosion());
    }

    void hide() const noexcept
    {
        context.bombTimerContainerPanel().hide();
    }

    Context context;
};
