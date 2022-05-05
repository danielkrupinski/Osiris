#pragma once

#include <cstdint>

#include <InventoryChanger/Backend/Item.h>

enum class Team;

namespace inventory_changer::backend::response
{

struct ItemAdded {
    ItemAdded(ItemConstIterator item, bool asUnacknowledged) : item{ item }, asUnacknowledged{ asUnacknowledged } {}

    ItemConstIterator item;
    bool asUnacknowledged;
};

struct ItemMovedToFront {
    explicit ItemMovedToFront(ItemConstIterator item) : item{ item } {}

    ItemConstIterator item;
};

struct ItemUpdated {
    explicit ItemUpdated(ItemConstIterator item) : item{ item } {}

    ItemConstIterator item;
};

struct ItemEquipped {
    ItemEquipped(ItemConstIterator item, std::uint8_t slot, Team team) : item{ item }, slot{ slot }, team{ team } {}

    ItemConstIterator item;
    std::uint8_t slot;
    Team team;
};

struct ItemRemoved {
    explicit ItemRemoved(std::uint64_t itemID) : itemID{ itemID } {}

    std::uint64_t itemID;
};

template <typename Tag>
struct StickerModified {
    StickerModified(ItemConstIterator skinItem, std::uint8_t stickerSlot) : skinItem{ skinItem }, stickerSlot{ stickerSlot } {}

    ItemConstIterator skinItem;
    std::uint8_t stickerSlot;
};

using StickerApplied = StickerModified<struct StickerAppliedTag>;
using StickerScraped = StickerModified<struct StickerScrapedTag>;
using StickerRemoved = StickerModified<struct StickerRemovedTag>;

struct StatTrakUpdated {
    StatTrakUpdated(std::uint64_t itemID, int newStatTrakValue) : itemID{ itemID }, newStatTrakValue{ newStatTrakValue } {}

    std::uint64_t itemID;
    int newStatTrakValue;
};

struct ViewerPassActivated {
    explicit ViewerPassActivated(ItemConstIterator createdEventCoin) : createdEventCoin{ createdEventCoin } {}

    ItemConstIterator createdEventCoin;
};

template <typename Tag>
struct NameTagModified {
    explicit NameTagModified(ItemConstIterator skinItem) : skinItem{ skinItem } {}

    ItemConstIterator skinItem;
};

using NameTagAdded = NameTagModified<struct NameTagAddedTag>;
using NameTagRemoved = NameTagModified<struct NameTagRemovedTag>;

struct ContainerOpened {
    explicit ContainerOpened(ItemConstIterator receivedItem) : receivedItem{ receivedItem } {}

    ItemConstIterator receivedItem;
};

template <typename Tag>
struct PatchModified {
    PatchModified(ItemConstIterator agentItem, std::uint8_t patchSlot) : agentItem{ agentItem }, patchSlot{ patchSlot } {}

    ItemConstIterator agentItem;
    std::uint8_t patchSlot;
};

using PatchApplied = PatchModified<struct PatchAppliedTag>;
using PatchRemoved = PatchModified<struct PatchRemovedTag>;

struct SouvenirTokenActivated {
    explicit SouvenirTokenActivated(ItemConstIterator tournamentCoin) : tournamentCoin{ tournamentCoin } {}

    ItemConstIterator tournamentCoin;
};

struct GraffitiUnsealed {
    explicit GraffitiUnsealed(ItemConstIterator graffitiItem) : graffitiItem{ graffitiItem } {}

    ItemConstIterator graffitiItem;
};

struct StatTrakSwapped {
    StatTrakSwapped(ItemConstIterator swapSourceItem, ItemConstIterator swapDestinationItem) : swapSourceItem{ swapSourceItem }, swapDestinationItem{ swapDestinationItem } {}

    ItemConstIterator swapSourceItem;
    ItemConstIterator swapDestinationItem;
};

struct TeamGraffitiSelected {
    TeamGraffitiSelected(ItemConstIterator tournamentCoin, std::uint16_t graffitiID) : tournamentCoin{ tournamentCoin }, graffitiID{ graffitiID } {}

    ItemConstIterator tournamentCoin;
    std::uint16_t graffitiID;
};

}
