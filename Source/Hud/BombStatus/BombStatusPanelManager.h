#pragma once

#include <Common/Visibility.h>

template <typename Context>
class BombStatusPanelManager {
public:
    explicit BombStatusPanelManager(Context context) noexcept
        : context{context}
    {
    }

    void run() const noexcept
    {
        if (updateFeaturesReplacingBombStatusPanel() == Visibility::Visible)
            context.bombStatusPanel().hide();
        else
            context.bombStatusPanel().restore();
    }

private:
    [[nodiscard]] Visibility updateFeaturesReplacingBombStatusPanel() const noexcept
    {
        if (context.postRoundTimer().update() == Visibility::Visible) {
            context.bombTimer().forceHide();
            return Visibility::Visible;
        }
        return context.bombTimer().update();
    }

    Context context;
};
