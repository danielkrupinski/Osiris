#pragma once

#include <cstdint>
#include <list>
#include <variant>

#include <InventoryChanger/Inventory/Item.h>

namespace inventory_changer::backend
{

struct Response {
    struct ItemAdded {
        std::list<inventory::Item_v2>::const_iterator item;
    };

    struct ItemMovedToFront {
        std::uint64_t itemID;
    };

    struct ItemRemoved {
        std::uint64_t itemID;
    };

    struct StickerApplied {
        std::list<inventory::Item_v2>::const_iterator skinItem;
        std::uint8_t stickerSlot;
    };

    struct StickerScraped {
        std::list<inventory::Item_v2>::const_iterator skinItem;
        std::uint8_t stickerSlot;
    };

    struct StatTrakUpdated {
        std::uint64_t itemID;
        int newStatTrakValue;
    };

    struct ViewerPassActivated {
        std::list<inventory::Item_v2>::const_iterator createdEventCoin;
    };

    std::variant<
        ItemAdded,
        ItemMovedToFront,
        ItemRemoved,
        StickerApplied,
        StickerScraped,
        StatTrakUpdated,
        ViewerPassActivated
    > data;
};

}
