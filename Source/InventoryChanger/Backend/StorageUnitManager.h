#pragma once

#include <cstdint>
#include <optional>
#include <unordered_map>

#include "Item.h"

namespace inventory_changer::backend
{

class StorageUnitManager {
public:
    void addItemToStorageUnit(ItemIterator item, ItemIterator storageUnit)
    {
        itemsStorageUnits[item] = storageUnit;
    }

    bool removeItemFromStorageUnit(ItemIterator item, ItemIterator storageUnit)
    { 
        if (const auto it = itemsStorageUnits.find(item); it != itemsStorageUnits.end() && it->second == storageUnit) {
            itemsStorageUnits.erase(it);
            return true;
        }
        return false;
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
