#pragma once

#include <cassert>
#include <cstdint>
#include <optional>
#include <unordered_map>

#include "Item.h"

namespace inventory_changer::backend
{

class StorageUnitManager {
public:
    bool addItemToStorageUnit(ItemIterator item, ItemIterator storageUnit)
    {
        assert(!item->gameItem().isStorageUnit() && storageUnit->gameItem().isStorageUnit());
        return itemsStorageUnits.try_emplace(item, storageUnit).second;
    }

    bool removeItemFromStorageUnit(ItemIterator item, ItemIterator storageUnit)
    {
        assert(!item->gameItem().isStorageUnit() && storageUnit->gameItem().isStorageUnit());

        if (const auto it = itemsStorageUnits.find(item); it != itemsStorageUnits.end() && it->second == storageUnit) {
            itemsStorageUnits.erase(it);
            return true;
        }
        return false;
    }

    std::optional<ItemIterator> removeItemFromStorageUnit(ItemIterator item)
    {
        assert(!item->gameItem().isStorageUnit());

        if (const auto it = itemsStorageUnits.find(item); it != itemsStorageUnits.end()) {
            const auto storageUnit = it->second;
            itemsStorageUnits.erase(it);
            return storageUnit;
        }
        return std::nullopt;
    }

    void removeStorageUnit(ItemIterator storageUnit)
    {
        assert(storageUnit->gameItem().isStorageUnit());
        std::erase_if(itemsStorageUnits, [storageUnit](const auto& pair) { return pair.second == storageUnit; });
    }

    [[nodiscard]] std::unordered_map<ItemIterator, std::uint32_t> getStorageUnitIDs() const
    {
        std::unordered_map<ItemIterator, std::uint32_t> itemsStorageUnitIDs;

        std::uint32_t highestStorageUnitID = 0;
        for (const auto& [item, storageUnitContainingItem] : itemsStorageUnits) {
            if (const auto it = itemsStorageUnitIDs.find(storageUnitContainingItem); it != itemsStorageUnitIDs.end()) {
                itemsStorageUnitIDs.emplace(item, it->second);
            } else {
                ++highestStorageUnitID;
                itemsStorageUnitIDs.emplace(storageUnitContainingItem, highestStorageUnitID);
                itemsStorageUnitIDs.emplace(item, highestStorageUnitID);
            }
        }

        return itemsStorageUnitIDs;
    }

    template <typename Function>
    void forEachItemInStorageUnit(ItemIterator storageUnit, Function function) const
    {
        assert(storageUnit->gameItem().isStorageUnit());

        for (const auto& [item, storageUnitContainingItem] : itemsStorageUnits) {
            if (storageUnitContainingItem == storageUnit)
                function(item);
        }
    }

    template <typename StoredItemRemoved>
    [[nodiscard]] std::optional<ItemIterator> onItemRemoval(ItemIterator item, const StoredItemRemoved& storedItemRemoved)
    {
        for (auto it = itemsStorageUnits.begin(); it != itemsStorageUnits.end();) {
            if (it->first == item) {
                const auto storageUnit = it->second;
                itemsStorageUnits.erase(it);
                return storageUnit;
            }

            if (it->second == item) {
                const auto storedItem = it->first;
                it = itemsStorageUnits.erase(it);
                storedItemRemoved(storedItem);
            } else {
                ++it;
            }
        }

        return {};
    }

private:
    std::unordered_map<ItemIterator, ItemIterator> itemsStorageUnits;
};

}
