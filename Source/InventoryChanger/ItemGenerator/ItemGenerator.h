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
    [[nodiscard]] std::optional<inventory::Item> generateItemFromContainer(Helpers::RandomGenerator& randomGenerator, const game_items::Lookup& gameItemLookup, const game_items::CrateLootLookup& crateLootLookup, const inventory::Item& caseItem, const inventory::Item* crateKey) noexcept;
    inventory::Item::Properties createDefaultItemProperties(Helpers::RandomGenerator& randomGenerator, const game_items::Storage& gameItemStorage, const game_items::Item& item) noexcept;
}

[[nodiscard]] std::time_t getStartOfYearTimestamp(std::uint16_t year) noexcept;
[[nodiscard]] std::time_t getEndOfYearTimestamp(std::uint16_t year) noexcept;
