#pragma once

#include <cstdint>

struct PlayerInfoPanelCachePerHookState {
    void reset() noexcept
    {
        nextEntryIndex = 0;
    }

    std::uint8_t nextEntryIndex{};
};
