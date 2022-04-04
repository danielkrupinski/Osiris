#pragma once

#include <list>
#include <optional>
#include <unordered_map>

#include <InventoryChanger/Inventory/Item.h> 

namespace inventory_changer::backend
{

class ItemIDMap {
public:
    [[nodiscard]] std::optional<std::list<inventory::Item_v2>::const_iterator> get(std::uint64_t itemID) const
    {
        const auto it = itemIDs.find(itemID);
        return it != itemIDs.end() ? std::make_optional(it->second) : std::nullopt;
    }

    [[nodiscard]] std::optional<std::uint64_t> getItemID(std::list<inventory::Item_v2>::const_iterator iterator) const
    {
        if (const auto it = std::ranges::find(itemIDs, iterator, &decltype(itemIDs)::value_type::second); it != itemIDs.end())
            return it->first;
        return {};
    }

    void add(std::uint64_t itemID, std::list<inventory::Item_v2>::const_iterator iterator)
    {
        itemIDs.emplace(itemID, iterator);
    }

    std::optional<std::uint64_t> remove(std::list<inventory::Item_v2>::const_iterator iterator)
    {
        if (const auto it = std::ranges::find(itemIDs, iterator, &decltype(itemIDs)::value_type::second); it != itemIDs.end()) {
            const auto itemID = it->first;
            itemIDs.erase(it);
            return itemID;
        }
        return {};
    }

private:
    std::unordered_map<std::uint64_t, std::list<inventory::Item_v2>::const_iterator> itemIDs;
};

}
