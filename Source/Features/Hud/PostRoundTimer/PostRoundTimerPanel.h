#pragma once

#include <CS2/Constants/ColorConstants.h>
#include <GameClient/Entities/TeamNumber.h>
#include <Utils/StringBuilder.h>

template <typename Context>
struct PostRoundTimerPanel {
    explicit PostRoundTimerPanel(Context context) noexcept
        : context{context}
    {
    }

    void showAndUpdate() const noexcept
    {
        context.countdownContainerPanel().show();

        auto&& countdownTextPanel = context.countdownTextPanel();
        updateCountdownTime(countdownTextPanel);
        countdownTextPanel.uiPanel().setColor(getColor());
    }

    void hide() const noexcept
    {
        context.countdownContainerPanel().hide();
    }

private:
    [[nodiscard]] auto getColor() const noexcept
    {
        switch (context.localPlayerTeamNumber()) {
        using enum TeamNumber;
        case TT: return cs2::kColorTeamTT;
        case CT: return cs2::kColorTeamCT;
        default: return cs2::kColorSilver;
        }
    }

    void updateCountdownTime(auto&& countdownTextPanel) const noexcept
    {
        const auto timeToRoundRestart = getTimeToRoundRestart();
        countdownTextPanel.setText(StringBuilderStorage<10>{}.builder().put(static_cast<int>(timeToRoundRestart), '.', static_cast<int>(timeToRoundRestart * 10) % 10).cstring());
    }

    [[nodiscard]] float getTimeToRoundRestart() const noexcept
    {
        if (auto&& timeToRoundRestart = context.gameRules().timeToRoundRestart(); timeToRoundRestart.hasValue() && timeToRoundRestart.value() >= 0.0f)
            return timeToRoundRestart.value();
        return 0.0f;
    }

    Context context;
};
