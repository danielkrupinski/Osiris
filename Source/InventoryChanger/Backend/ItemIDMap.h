#pragma once

#include <functional>
#include <list>
#include <optional>
#include <unordered_map>

#include <InventoryChanger/Inventory/Item.h> 
#include <InventoryChanger/ItemId.h> 

#include "Item.h"

namespace inventory_changer::backend
{

class ItemIDMap {
public:
    [[nodiscard]] std::optional<ItemIterator> get(ItemId itemID) const
    {
        const auto it = itemIDsIterators.find(itemID);
        return it != itemIDsIterators.end() ? std::make_optional(it->second) : std::nullopt;
    }

    [[nodiscard]] std::optional<ItemId> getItemID(ItemIterator iterator) const
    {
        const auto it = iteratorsItemIDs.find(iterator);
        return it != iteratorsItemIDs.end() ? std::make_optional(it->second) : std::nullopt;
    }

    void add(ItemId itemID, ItemIterator iterator)
    {
        if (const bool inserted = iteratorsItemIDs.try_emplace(iterator, itemID).second)
            itemIDsIterators.try_emplace(itemID, iterator);
    }

    void update(ItemId oldItemID, ItemId newItemID)
    {
        if (const auto itemIDToIterator = itemIDsIterators.find(oldItemID); itemIDToIterator != itemIDsIterators.end()) {
            if (const auto iteratorToItemID = iteratorsItemIDs.find(itemIDToIterator->second); iteratorToItemID != iteratorsItemIDs.end())
                iteratorToItemID->second = newItemID;

            auto node = itemIDsIterators.extract(itemIDToIterator);
            node.key() = newItemID;
            itemIDsIterators.insert(std::move(node));
        }
    }

    std::optional<ItemId> remove(ItemIterator iterator)
    {
        if (const auto it = iteratorsItemIDs.find(iterator); it != iteratorsItemIDs.end()) {
            const auto itemID = it->second;
            iteratorsItemIDs.erase(it);
            itemIDsIterators.erase(itemID);
            return itemID;
        }
        return {};
    }

private:
    std::unordered_map<ItemId, ItemIterator> itemIDsIterators;
    std::unordered_map<ItemIterator, ItemId> iteratorsItemIDs;
};

}
