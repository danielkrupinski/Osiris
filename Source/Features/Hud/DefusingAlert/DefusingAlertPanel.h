#pragma once

template <typename Context>
struct DefusingAlertPanel {
    void showAndUpdate(const auto bomb) const noexcept
    {
        context.defusingAlertContainerPanel().show();
        context.defusingCountdownTextPanel()
            .setTimeToDefuseEnd(bomb.getTimeToDefuseEnd())
            .setCanBeDefused(bomb.canBeDefused());
    }

    void hide() const noexcept
    {
        context.defusingAlertContainerPanel().hide();
    }

    Context context;
};
