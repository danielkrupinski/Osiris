#pragma once

#include <functional>
#include <list>
#include <memory>

#include <InventoryChanger/Inventory/Item.h>

namespace inventory_changer::backend
{
    using ItemList = std::list<inventory::Item>;
    using ItemMutableIterator = ItemList::iterator;
    using ItemIterator = ItemList::const_iterator;

    struct ItemConstRemover {
        explicit ItemConstRemover(ItemList& itemList) : itemList{ itemList } {}

        [[nodiscard]] inventory::Item& operator()(ItemIterator it) const noexcept
        {
            return *itemList.erase(it, it);
        }

    private:
        ItemList& itemList;
    };
}

template <>
struct std::hash<inventory_changer::backend::ItemIterator> {
    [[nodiscard]] std::size_t operator()(inventory_changer::backend::ItemIterator iterator) const noexcept
    {
        return std::hash<const inventory_changer::inventory::Item*>{}(std::to_address(iterator));
    }
};
