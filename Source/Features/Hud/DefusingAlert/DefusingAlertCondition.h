#pragma once

template <typename Context>
struct DefusingAlertCondition {
    explicit DefusingAlertCondition(Context context) noexcept
        : context{context}
    {
    }

    [[nodiscard]] bool shouldRun() const noexcept
    {
        return context.state().enabled;
    }

    [[nodiscard]] bool shouldShowDefuseAlert() const noexcept
    {
        return context.hasC4BeingDefused();
    }

private:
    Context context;
};
