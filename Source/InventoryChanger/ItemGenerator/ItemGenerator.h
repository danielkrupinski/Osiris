#pragma once

#include <cstddef>
#include <ctime>
#include <optional>
#include <utility>

#include <InventoryChanger/GameItems/CrateLootLookup.h>
#include <InventoryChanger/GameItems/Lookup.h>
#include <InventoryChanger/GameItems/Storage.h>
#include <InventoryChanger/Inventory/Item.h>

namespace inventory_changer::item_generator
{
    [[nodiscard]] std::optional<inventory::Item> generateItemFromContainer(const game_items::Lookup& gameItemLookup, const game_items::CrateLootLookup& crateLootLookup, const inventory::Item& caseItem) noexcept;
    inventory::ItemData createDefaultDynamicData(const game_items::Storage& gameItemStorage, const game_items::Item& item) noexcept;
}

[[nodiscard]] std::time_t tmToUTCTimestamp(std::tm& tm) noexcept;
[[nodiscard]] std::time_t getStartOfYearTimestamp(std::uint16_t year) noexcept;
[[nodiscard]] std::time_t getEndOfYearTimestamp(std::uint16_t year) noexcept;
