#pragma once

#include <CS2/Classes/Color.h>
#include <CS2/Constants/ColorConstants.h>
#include <Utils/Optional.h>
#include <Utils/StringBuilder.h>

template <typename LabelPanel>
struct DefusingCountdownTextPanel {
    explicit DefusingCountdownTextPanel(LabelPanel panel) noexcept
        : panel{panel}
    {
    }

    auto& setTimeToDefuseEnd(Optional<float> timeToDefuseEnd) const noexcept
    {
        if (timeToDefuseEnd.hasValue()) {
            panel.setText(StringBuilderStorage<10>{}.builder().put(
                static_cast<int>(timeToDefuseEnd.value()), '.', static_cast<int>(timeToDefuseEnd.value() * 10) % 10).cstring());
        }
        return *this;
    }

    auto& setCanBeDefused(Optional<bool> canBeDefused) const noexcept
    {
        panel.uiPanel().setColor(getDefusingCountdownColor(canBeDefused));
        return *this;
    }

private:
    [[nodiscard]] static cs2::Color getDefusingCountdownColor(Optional<bool> canBeDefused) noexcept
    {
        if (canBeDefused.hasValue())
            return canBeDefused.value() ? cs2::kColorGreen : cs2::kColorRed;
        return cs2::kColorWhite;
    }

    LabelPanel panel;
};
