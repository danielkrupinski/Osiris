#pragma once

#include <cstdint>

#include "Item.h"

enum class Team;

namespace inventory_changer::backend::response
{

struct ItemAdded {
    ItemAdded(ItemConstIterator item, bool asUnacknowledged) : item{ item }, asUnacknowledged{ asUnacknowledged } {}

    ItemConstIterator item;
    bool asUnacknowledged;
};

struct ItemMovedToFront {
    explicit ItemMovedToFront(std::uint64_t itemID) : itemID{ itemID } {}

    std::uint64_t itemID;
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

struct StickerApplied {
    StickerApplied(ItemConstIterator skinItem, std::uint8_t stickerSlot) : skinItem{ skinItem }, stickerSlot{ stickerSlot } {}

    ItemConstIterator skinItem;
    std::uint8_t stickerSlot;
};

struct StickerScraped {
    StickerScraped(ItemConstIterator skinItem, std::uint8_t stickerSlot) : skinItem{ skinItem }, stickerSlot{ stickerSlot } {}

    ItemConstIterator skinItem;
    std::uint8_t stickerSlot;
};

struct StickerRemoved {
    StickerRemoved(ItemConstIterator skinItem, std::uint8_t stickerSlot) : skinItem{ skinItem }, stickerSlot{ stickerSlot } {}

    ItemConstIterator skinItem;
    std::uint8_t stickerSlot;
};

struct StatTrakUpdated {
    StatTrakUpdated(std::uint64_t itemID, int newStatTrakValue) : itemID{ itemID }, newStatTrakValue{ newStatTrakValue } {}

    std::uint64_t itemID;
    int newStatTrakValue;
};

struct ViewerPassActivated {
    explicit ViewerPassActivated(ItemConstIterator createdEventCoin) : createdEventCoin{ createdEventCoin } {}

    ItemConstIterator createdEventCoin;
};

struct NameTagAdded {
    explicit NameTagAdded(ItemConstIterator skinItem) : skinItem{ skinItem } {}

    ItemConstIterator skinItem;
};

struct NameTagRemoved {
    explicit NameTagRemoved(ItemConstIterator skinItem) : skinItem{ skinItem } {}

    ItemConstIterator skinItem;
};

struct ContainerOpened {
    explicit ContainerOpened(ItemConstIterator receivedItem) : receivedItem{ receivedItem } {}

    ItemConstIterator receivedItem;
};

struct PatchApplied {
    PatchApplied(ItemConstIterator agentItem, std::uint8_t patchSlot) : agentItem{ agentItem }, patchSlot{ patchSlot } {}

    ItemConstIterator agentItem;
    std::uint8_t patchSlot;
};

struct PatchRemoved {
    PatchRemoved(ItemConstIterator agentItem, std::uint8_t patchSlot) : agentItem{ agentItem }, patchSlot{ patchSlot } {}

    ItemConstIterator agentItem;
    std::uint8_t patchSlot;
};

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

}
