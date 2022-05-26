#pragma once

#include <cassert>
#include <functional>
#include <span>
#include <vector>

#include <InventoryChanger/EconRarities.h>

namespace inventory_changer::game_items
{

struct Item;

class CrateLoot {
public:
    struct LootList {
        LootList(std::uint16_t crateSeries, std::size_t lootBeginIdx)
            : crateSeries{ crateSeries }, lootBeginIdx{ lootBeginIdx }, lootEndIdx{ lootBeginIdx } {}

        std::uint16_t crateSeries = 0;
        bool willProduceStatTrak = false;
        EconRarities rarities{};
        std::size_t lootBeginIdx = 0;
        std::size_t lootEndIdx = 0;
    };

    void nextLootList(std::uint16_t crateSeries)
    {
        assert(std::ranges::find(lootLists, crateSeries, &LootList::crateSeries) == lootLists.end());
        lootLists.emplace_back(crateSeries, items.size());
    }

    void nextLootListFromPrevious(std::uint16_t crateSeries)
    {
        assert(std::ranges::find(lootLists, crateSeries, &LootList::crateSeries) == lootLists.end());
        assert(!lootLists.empty());
        lootLists.emplace_back(lootLists.back()).crateSeries = crateSeries;
    }

    void addItem(std::reference_wrapper<const Item> item)
    {
        assert(!lootLists.empty());
        items.push_back(item);
        ++lootLists.back().lootEndIdx;
    }

    void setWillProduceStatTrak()
    {
        assert(!lootLists.empty());
        lootLists.back().willProduceStatTrak = true;
    }

    void compress()
    {
        items.shrink_to_fit();
        lootLists.shrink_to_fit();
    }

    [[nodiscard]] std::span<std::reference_wrapper<const Item>> getItems() noexcept
    {
        return items;
    }

    [[nodiscard]] std::span<const std::reference_wrapper<const Item>> getItems() const noexcept
    {
        return items;
    }

    [[nodiscard]] std::span<LootList> getLootLists() noexcept
    {
        return lootLists;
    }

    [[nodiscard]] std::span<const LootList> getLootLists() const noexcept
    {
        return lootLists;
    }

private:
    std::vector<std::reference_wrapper<const Item>> items;
    std::vector<LootList> lootLists;
};

}
