#pragma once

#include <cstddef>
#include <ctime>
#include <utility>

#include "StaticData.h"

#include "Inventory/Item.h"

namespace ItemGenerator
{
    inventory::Item generateItemFromContainer(const inventory::Item& caseItem) noexcept;
    inventory::ItemData createDefaultDynamicData(const game_items::Item& item) noexcept;
}

[[nodiscard]] std::time_t tmToUTCTimestamp(std::tm& tm) noexcept;
[[nodiscard]] std::time_t getStartOfYearTimestamp(std::uint16_t year) noexcept;
[[nodiscard]] std::time_t getEndOfYearTimestamp(std::uint16_t year) noexcept;
