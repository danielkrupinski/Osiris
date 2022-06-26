#pragma once

#include <functional>
#include <list>
#include <optional>
#include <unordered_map>

#include <InventoryChanger/Inventory/Item.h> 

#include "Item.h"

namespace inventory_changer::backend
{

class ItemIDMap {
public:
    [[nodiscard]] std::optional<ItemIterator> get(std::uint64_t itemID) const
    {
        const auto it = itemIDsIterators.find(itemID);
        return it != itemIDsIterators.end() ? std::make_optional(it->second) : std::nullopt;
    }

    [[nodiscard]] std::optional<std::uint64_t> getItemID(ItemIterator iterator) const
    {
        const auto it = iteratorsItemIDs.find(iterator);
        return it != iteratorsItemIDs.end() ? std::make_optional(it->second) : std::nullopt;
    }

    void add(std::uint64_t itemID, ItemIterator iterator)
    {
        if (const bool inserted = iteratorsItemIDs.try_emplace(iterator, itemID).second)
            itemIDsIterators.try_emplace(itemID, iterator);
    }

    void update(std::uint64_t oldItemID, std::uint64_t newItemID)
    {
        if (const auto itemIDToIterator = itemIDsIterators.find(oldItemID); itemIDToIterator != itemIDsIterators.end()) {
            if (const auto iteratorToItemID = iteratorsItemIDs.find(itemIDToIterator->second); iteratorToItemID != iteratorsItemIDs.end())
                iteratorToItemID->second = newItemID;

            auto node = itemIDsIterators.extract(itemIDToIterator);
            node.key() = newItemID;
            itemIDsIterators.insert(std::move(node));
        }
    }

    std::optional<std::uint64_t> remove(ItemIterator iterator)
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
    std::unordered_map<std::uint64_t, ItemIterator> itemIDsIterators;
    std::unordered_map<ItemIterator, std::uint64_t> iteratorsItemIDs;
};

}
