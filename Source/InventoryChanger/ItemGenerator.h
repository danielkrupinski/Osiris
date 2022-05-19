#pragma once

#include <cstddef>
#include <ctime>
#include <optional>
#include <utility>

#include "StaticData.h"

#include "GameItems/Storage.h"
#include "Inventory/Item.h"

namespace ItemGenerator
{
    [[nodiscard]] std::optional<inventory::Item> generateItemFromContainer(const game_items::Storage& gameItemStorage, const inventory::Item& caseItem) noexcept;
    inventory::ItemData createDefaultDynamicData(const game_items::Item& item) noexcept;
}

[[nodiscard]] std::time_t tmToUTCTimestamp(std::tm& tm) noexcept;
[[nodiscard]] std::time_t getStartOfYearTimestamp(std::uint16_t year) noexcept;
[[nodiscard]] std::time_t getEndOfYearTimestamp(std::uint16_t year) noexcept;
