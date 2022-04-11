#pragma once

#include <cstdint>
#include <list>
#include <variant>

#include <InventoryChanger/Inventory/Item.h>

namespace inventory_changer::backend
{

struct Response {
    struct ItemAdded {
        std::list<inventory::Item>::const_iterator item;
        bool asUnacknowledged;
    };

    struct ItemMovedToFront {
        std::uint64_t itemID;
    };

    struct ItemRemoved {
        std::uint64_t itemID;
    };

    struct StickerApplied {
        std::list<inventory::Item>::const_iterator skinItem;
        std::uint8_t stickerSlot;
    };

    struct StickerScraped {
        std::list<inventory::Item>::const_iterator skinItem;
        std::uint8_t stickerSlot;
    };

    struct StickerRemoved {
        std::list<inventory::Item>::const_iterator skinItem;
        std::uint8_t stickerSlot;
    };

    struct StatTrakUpdated {
        std::uint64_t itemID;
        int newStatTrakValue;
    };

    struct ViewerPassActivated {
        std::list<inventory::Item>::const_iterator createdEventCoin;
    };

    struct NameTagAdded {
        std::list<inventory::Item>::const_iterator skinItem;
    };

    struct NameTagRemoved {
        std::list<inventory::Item>::const_iterator skinItem;
    };

    struct ContainerOpened {
        std::list<inventory::Item>::const_iterator receivedItem;
    };

    struct PatchApplied {
        std::list<inventory::Item>::const_iterator agentItem;
        std::uint8_t patchSlot;
    };

    struct PatchRemoved {
        std::list<inventory::Item>::const_iterator agentItem;
        std::uint8_t patchSlot;
    };

    struct SouvenirTokenActivated {
        std::list<inventory::Item>::const_iterator tournamentCoin;
    };

    struct GraffitiUnsealed {
        std::list<inventory::Item>::const_iterator graffitiItem;
    };

    struct StatTrakSwapped {
        std::list<inventory::Item>::const_iterator swapSourceItem;
        std::list<inventory::Item>::const_iterator swapDestinationItem;
    };

    std::variant<
        ItemAdded,
        ItemMovedToFront,
        ItemRemoved,
        StickerApplied,
        StickerScraped,
        StickerRemoved,
        StatTrakUpdated,
        ViewerPassActivated,
        NameTagAdded,
        NameTagRemoved,
        ContainerOpened,
        PatchApplied,
        PatchRemoved,
        SouvenirTokenActivated,
        GraffitiUnsealed,
        StatTrakSwapped
    > data;
};

}
