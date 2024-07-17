#pragma once

template <typename Context>
struct DefusingAlertPanel {
    explicit DefusingAlertPanel(Context context) noexcept
        : context{context}
    {
    }

    void showAndUpdate() const noexcept
    {
        context.defusingAlertContainerPanel().show();
        decltype(auto) c4BeingDefused{context.c4BeingDefused()};
        context.defusingCountdownTextPanel()
            .setTimeToDefuseEnd(c4BeingDefused.getTimeToDefuseEnd())
            .setCanBeDefused(c4BeingDefused.canBeDefused());
    }

    void hide() const noexcept
    {
        context.defusingAlertContainerPanel().hide();
    }

private:
    Context context;
};
