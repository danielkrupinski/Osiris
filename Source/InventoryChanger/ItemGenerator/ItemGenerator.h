#pragma once

#include <cstddef>
#include <ctime>
#include <optional>
#include <utility>

#include <InventoryChanger/GameItems/CrateLootLookup.h>
#include <InventoryChanger/GameItems/Lookup.h>
#include <InventoryChanger/GameItems/Storage.h>
#include <InventoryChanger/Inventory/Item.h>

namespace inventory_changer
{

class ItemGenerator {
public:
    ItemGenerator(const Memory& memory, Helpers::RandomGenerator& randomGenerator, const game_items::Lookup& gameItemLookup, const game_items::CrateLootLookup& crateLootLookup)
        : memory{ memory }, randomGenerator{ randomGenerator }, gameItemLookup{ gameItemLookup }, crateLootLookup{ crateLootLookup } {}

    [[nodiscard]] std::optional<inventory::Item> generateItemFromContainer(const inventory::Item& caseItem, const inventory::Item* crateKey) const noexcept;
    inventory::Item::Properties createDefaultItemProperties(const game_items::Item& item) const noexcept;

private:
    const Memory& memory;
    Helpers::RandomGenerator& randomGenerator;
    const game_items::Lookup& gameItemLookup;
    const game_items::CrateLootLookup& crateLootLookup;
};

}

[[nodiscard]] std::time_t getStartOfYearTimestamp(std::uint16_t year) noexcept;
[[nodiscard]] std::time_t getEndOfYearTimestamp(std::uint16_t year) noexcept;
