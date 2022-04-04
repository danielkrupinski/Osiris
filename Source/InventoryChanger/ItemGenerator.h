#pragma once

#include <cstddef>
#include <ctime>
#include <utility>

#include "Inventory.h"
#include "StaticData.h"

#include "Inventory/Item.h"

struct InventoryItem;

namespace ItemGenerator
{
    inventory::Item_v2 generateItemFromContainer(const inventory::Item_v2& caseItem) noexcept;
    inventory::StructWrapper createDefaultDynamicData(const game_items::Item& item) noexcept;
}

[[nodiscard]] std::time_t tmToUTCTimestamp(std::tm& tm) noexcept;
[[nodiscard]] std::time_t getStartOfYearTimestamp(std::uint16_t year) noexcept;
[[nodiscard]] std::time_t getEndOfYearTimestamp(std::uint16_t year) noexcept;
