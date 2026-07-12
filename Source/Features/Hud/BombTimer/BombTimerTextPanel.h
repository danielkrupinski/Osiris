#pragma once

#include <span>

#include <Utils/StringBuilder.h>

template <typename LabelPanel>
struct BombTimerTextPanel {
    BombTimerTextPanel(LabelPanel panel, std::span<char> textBuffer) noexcept
        : panel{panel}
        , textBuffer{textBuffer}
    {
    }

    void setTimeToExplosion(float timeToExplosion) const noexcept
    {
        StringBuilder builder{textBuffer};
        builder.put(static_cast<int>(timeToExplosion), '.', static_cast<int>(timeToExplosion * 10) % 10);
        panel.setText(builder.cstring());
    }

    LabelPanel panel;
    std::span<char> textBuffer;
};
