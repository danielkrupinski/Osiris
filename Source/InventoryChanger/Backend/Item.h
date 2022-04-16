#pragma once

#include <functional>
#include <list>
#include <memory>

namespace inventory
{
    class Item;
}

namespace inventory_changer::backend
{
    using ItemList = std::list<inventory::Item>;
    using ItemIterator = ItemList::iterator;
    using ItemConstIterator = ItemList::const_iterator;
}

template <>
struct std::hash<inventory_changer::backend::ItemConstIterator> {
    [[nodiscard]] std::size_t operator()(inventory_changer::backend::ItemConstIterator iterator) const noexcept
    {
        return std::hash<const inventory::Item*>{}(std::to_address(iterator));
    }
};
