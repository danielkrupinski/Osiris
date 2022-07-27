#pragma once

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
                itemsStorageUnits.erase(it);
                return it->second;
            }

            if (it->second == item) {
                storedItemRemoved(it->first);
                it = itemsStorageUnits.erase(it);
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
