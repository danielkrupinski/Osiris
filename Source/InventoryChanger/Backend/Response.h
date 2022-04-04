#pragma once

#include <cstdint>
#include <list>
#include <variant>

#include <InventoryChanger/Inventory/Item.h>

namespace inventory_changer::backend
{

struct Response {
    enum class Type {
        ItemAdded,
        ItemRemoved,
        StatTrakUpdated,
        StickerApplied
    };

    struct StickerApplied {
        std::list<inventory::Item_v2>::const_iterator skinItem;
        std::uint8_t stickerSlot;
    };

    Type type;
    std::variant<std::list<inventory::Item_v2>::const_iterator, StickerApplied, std::uint64_t> data;
};

}
