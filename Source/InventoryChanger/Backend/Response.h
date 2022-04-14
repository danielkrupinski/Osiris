#pragma once

#include <cstdint>
#include <list>
#include <variant>

#include <InventoryChanger/Inventory/Item.h>

#include "Item.h"

namespace inventory_changer::backend
{

struct Response {
    struct ItemAdded {
        ItemConstIterator item;
        bool asUnacknowledged;
    };

    struct ItemMovedToFront {
        std::uint64_t itemID;
    };

    struct ItemEquipped {
        ItemConstIterator item;
        std::uint8_t slot;
        Team team;
    };

    struct ItemRemoved {
        std::uint64_t itemID;
    };

    struct StickerApplied {
        ItemConstIterator skinItem;
        std::uint8_t stickerSlot;
    };

    struct StickerScraped {
        ItemConstIterator skinItem;
        std::uint8_t stickerSlot;
    };

    struct StickerRemoved {
        ItemConstIterator skinItem;
        std::uint8_t stickerSlot;
    };

    struct StatTrakUpdated {
        std::uint64_t itemID;
        int newStatTrakValue;
    };

    struct ViewerPassActivated {
        ItemConstIterator createdEventCoin;
    };

    struct NameTagAdded {
        ItemConstIterator skinItem;
    };

    struct NameTagRemoved {
        ItemConstIterator skinItem;
    };

    struct ContainerOpened {
        ItemConstIterator receivedItem;
    };

    struct PatchApplied {
        ItemConstIterator agentItem;
        std::uint8_t patchSlot;
    };

    struct PatchRemoved {
        ItemConstIterator agentItem;
        std::uint8_t patchSlot;
    };

    struct SouvenirTokenActivated {
        ItemConstIterator tournamentCoin;
    };

    struct GraffitiUnsealed {
        ItemConstIterator graffitiItem;
    };

    struct StatTrakSwapped {
        ItemConstIterator swapSourceItem;
        ItemConstIterator swapDestinationItem;
    };

    std::variant<
        ItemAdded,
        ItemMovedToFront,
        ItemEquipped,
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
