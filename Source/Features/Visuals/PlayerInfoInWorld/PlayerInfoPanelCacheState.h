#pragma once

#include <Utils/DynamicArray.h>

#include "PlayerInfoPanelCacheEntry.h"

struct PlayerInfoPanelCacheState {
    DynamicArray<PlayerInfoPanelCacheEntry> cache;
};
