#pragma once

#include <list>
#include <variant>

#include <InventoryChanger/Inventory/Item.h>

namespace inventory_changer::backend
{

struct Response {
    enum class Type {
        ItemAdded,
        ItemRemoved,
        StatTrakUpdated
    };

    Type type;
    std::variant<std::list<inventory::Item_v2>::const_iterator> data;
};

}
