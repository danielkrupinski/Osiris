#pragma once

#include <cstddef>
#include <ctime>
#include <utility>

#include "Inventory.h"
#include "StaticData.h"

struct InventoryItem;

namespace ItemGenerator
{
    std::pair<const game_items::Item&, std::size_t> generateItemFromContainer(const inventory::Item& caseItem) noexcept;
    std::size_t createDefaultDynamicData(const game_items::Item& item) noexcept;
}

[[nodiscard]] std::time_t tmToUTCTimestamp(std::tm& tm) noexcept;
[[nodiscard]] std::time_t getStartOfYearTimestamp(std::uint16_t year) noexcept;
[[nodiscard]] std::time_t getEndOfYearTimestamp(std::uint16_t year) noexcept;
