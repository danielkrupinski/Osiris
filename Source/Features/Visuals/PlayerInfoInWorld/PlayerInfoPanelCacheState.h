#pragma once

#include <array>

#include <Utils/DynamicArray.h>

#include "PlayerInfoPanelCacheEntry.h"

struct PlayerInfoPanelCacheState {
    static constexpr std::size_t kMaxEntries{64};

    DynamicArray<PlayerInfoPanelCacheEntry> cache;
    std::array<std::array<char, 10>, kMaxEntries> playerHealthText;
    std::array<std::array<char, 10>, kMaxEntries> activeWeaponAmmoText;
};
