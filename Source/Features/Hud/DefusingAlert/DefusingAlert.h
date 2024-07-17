#pragma once

template <typename Context>
class DefusingAlert {
public:
    explicit DefusingAlert(Context context) noexcept
        : context{context}
    {
    }

    void run() const noexcept
    {
        decltype(auto) condition{context.defusingAlertCondition()};
        if (!condition.shouldRun())
            return;

        if (condition.shouldShowDefuseAlert())
            context.defusingAlertPanel().showAndUpdate();
        else
            context.defusingAlertPanel().hide();
    }

private:
    Context context;
};
