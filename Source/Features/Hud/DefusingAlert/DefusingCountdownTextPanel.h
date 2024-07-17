#pragma once

#include <optional>

#include <CS2/Classes/Color.h>
#include <CS2/Constants/ColorConstants.h>
#include <Utils/StringBuilder.h>

template <typename LabelPanel>
struct DefusingCountdownTextPanel {
    explicit DefusingCountdownTextPanel(LabelPanel panel) noexcept
        : panel{panel}
    {
    }

    auto& setTimeToDefuseEnd(std::optional<float> timeToDefuseEnd) const noexcept
    {
        if (timeToDefuseEnd.has_value()) {
            panel.setText(StringBuilderStorage<10>{}.builder().put(
                static_cast<int>(*timeToDefuseEnd), '.', static_cast<int>(*timeToDefuseEnd * 10) % 10).cstring());
        }
        return *this;
    }

    auto& setCanBeDefused(std::optional<bool> canBeDefused) const noexcept
    {
        panel.setColor(getDefusingCountdownColor(canBeDefused));
        return *this;
    }

private:
    [[nodiscard]] static cs2::Color getDefusingCountdownColor(std::optional<bool> canBeDefused) noexcept
    {
        if (canBeDefused.has_value())
            return *canBeDefused ? cs2::kColorGreen : cs2::kColorRed;
        return cs2::kColorWhite;
    }

    LabelPanel panel;
};
