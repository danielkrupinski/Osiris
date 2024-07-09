#pragma once

#include <Utils/StringBuilder.h>

template <typename LabelPanel>
struct BombTimerTextPanel {
    explicit BombTimerTextPanel(LabelPanel panel) noexcept
        : panel{panel}
    {
    }

    void setTimeToExplosion(float timeToExplosion) const noexcept
    {
        StringBuilderStorage<10> storage;
        StringBuilder builder = storage.builder();
        builder.put(static_cast<int>(timeToExplosion), '.', static_cast<int>(timeToExplosion * 10) % 10);
        panel.setText(builder.cstring());
    }

    LabelPanel panel;
};
