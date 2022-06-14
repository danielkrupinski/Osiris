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

template <typename Tag>
struct ItemModified {
    explicit ItemModified(ItemConstIterator item) : item{ item } {}

    ItemConstIterator item;
};

using ItemMovedToFront = ItemModified<struct ItemMovedToFrontTag>;
using ItemUpdated = ItemModified<struct ItemUpdatedTag>;
using ItemHidden = ItemModified<struct ItemHiddenTag>;
using ItemUnhidden = ItemModified<struct ItemUnhiddenTag>;

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
    StatTrakUpdated(ItemConstIterator item, int newStatTrakValue) : item{ item }, newStatTrakValue{ newStatTrakValue } {}

    ItemConstIterator item;
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
    explicit StatTrakSwapped(ItemConstIterator itemWithHigherStatTrakAfterSwap)
        : itemWithHigherStatTrakAfterSwap{ itemWithHigherStatTrakAfterSwap } {}

    ItemConstIterator itemWithHigherStatTrakAfterSwap;
};

struct TeamGraffitiSelected {
    TeamGraffitiSelected(ItemConstIterator tournamentCoin, std::uint16_t graffitiID) : tournamentCoin{ tournamentCoin }, graffitiID{ graffitiID } {}

    ItemConstIterator tournamentCoin;
    std::uint16_t graffitiID;
};

struct PickEmUpdated {};

struct XRayScannerUsed {
    explicit XRayScannerUsed(ItemConstIterator receivedItem) : receivedItem{ receivedItem } {}

    ItemConstIterator receivedItem;
};

struct XRayItemClaimed {
    explicit XRayItemClaimed(ItemConstIterator item) : item{ item } {}

    ItemConstIterator item;
};

}
