#pragma once

#include <algorithm>
#include <memory>

#include <range/v3/algorithm/equal_range.hpp>

#include "CrateLoot.h"
#include "Item.h"

namespace inventory_changer::game_items
{

class CrateLootLookup {
public:
    CrateLootLookup() = default;

    explicit CrateLootLookup(CrateLoot loot) : loot{ std::move(loot) }
    {
        std::ranges::sort(this->loot.getLootLists(), {}, &CrateLoot::LootList::crateSeries);

        for (auto& lootList : this->loot.getLootLists()) {
            auto lootItems = getLoot(lootList);
            for (const Item& item : lootItems)
                lootList.rarities.set(item.getRarity());

            std::ranges::sort(lootItems, {}, [](const Item& item) { return item.getRarity(); });
        }
    }

    [[nodiscard]] const CrateLoot::LootList* findLootList(std::uint16_t crateSeries) const
    {
        const auto& lootLists = loot.getLootLists();
        if (const auto it = std::ranges::lower_bound(lootLists, crateSeries, {}, &CrateLoot::LootList::crateSeries); it != lootLists.end() && it->crateSeries == crateSeries)
            return std::to_address(it);
        return nullptr;
    }

    [[nodiscard]] std::span<const std::reference_wrapper<const Item>> getLoot(std::uint16_t crateSeries) const
    {
        if (const auto lootList = findLootList(crateSeries))
            return { loot.getItems().begin() + lootList->lootBeginIdx, loot.getItems().begin() + lootList->lootEndIdx };
        return {};
    }

private:
    [[nodiscard]] std::span<std::reference_wrapper<const Item>> getLoot(const CrateLoot::LootList& lootList)
    {
        return { loot.getItems().begin() + lootList.lootBeginIdx, loot.getItems().begin() + lootList.lootEndIdx };
    }

    CrateLoot loot;
};

[[nodiscard]] inline std::span<const std::reference_wrapper<const Item>> getLootOfRarity(const CrateLootLookup& lootLookup, std::uint16_t crateSeries, EconRarity rarity)
{
    return ranges::equal_range(lootLookup.getLoot(crateSeries), rarity, {}, [](const Item& item) { return item.getRarity(); });
}

}
