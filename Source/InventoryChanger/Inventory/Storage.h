#pragma once

#include <cassert>
#include <list>
#include <memory>
#include <variant>

#include <InventoryChanger/GameItems/Item.h>

#include "Item.h"
#include "Structs.h"

namespace inventory
{

class Storage {
public:
    void addItem(Item_v2 item)
    {
        items.push_back(std::move(item));
    }

    [[nodiscard]] auto begin()
    {
        return items.begin();
    }

    [[nodiscard]] auto end()
    {
        return items.end();
    }

    [[nodiscard]] auto begin() const
    {
        return items.begin();
    }

    [[nodiscard]] auto end() const
    {
        return items.end();
    }

    std::list<Item_v2>::iterator removeItem(std::list<Item_v2>::const_iterator it)
    {
        return items.erase(it);
    }

    void clear()
    {
        items.clear();
    }

    [[nodiscard]] std::size_t size() const noexcept
    {
        return items.size();
    }

private:
    std::list<Item_v2> items;
};

}
