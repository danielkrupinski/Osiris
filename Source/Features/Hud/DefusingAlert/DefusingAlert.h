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
        if (!context.state().enabled)
            return;

        if (const auto plantedC4{context.plantedC4()}; plantedC4 && plantedC4->isBeingDefused())
            context.defusingAlertPanel().showAndUpdate(*plantedC4);
        else
            context.defusingAlertPanel().hide();
    }

private:
    Context context;
};
