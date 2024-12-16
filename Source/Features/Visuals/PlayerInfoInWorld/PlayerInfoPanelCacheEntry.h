#pragma once

#include <CS2/Classes/Color.h>

struct PlayerInfoPanelCacheEntry {
    template <typename T>
    struct Cached {
        Cached() = default;
        Cached(T value) noexcept
            : cachedValue{value}
        {
        }

        [[nodiscard]] bool operator()(T currentValue) noexcept
        {
            if (cachedValue != currentValue) {
                cachedValue = currentValue;
                return true;
            }
            return false;
        }

    private:
        T cachedValue{};
    };

    Cached<cs2::Color> playerPositionArrowColor{cs2::Color{0, 0, 0, 0}};
    Cached<cs2::Color> playerHealthTextColor{cs2::Color{0, 0, 0, 0}};
    Cached<int> playerHealth{-1};
    Cached<int> activeWeaponAmmo{-1};
    Cached<float> opacity{-1.0f};
};
