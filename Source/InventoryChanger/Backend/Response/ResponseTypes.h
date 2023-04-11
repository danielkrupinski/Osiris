#pragma once

#include <cstdint>

#include <InventoryChanger/Backend/Item.h>
#include <InventoryChanger/ItemId.h>

enum class Team;

namespace inventory_changer::backend::response
{

struct ItemAdded {
    ItemAdded(ItemIterator item, bool asUnacknowledged) : item{ item }, asUnacknowledged{ asUnacknowledged } {}

    ItemIterator item;
    bool asUnacknowledged;
};

template <typename Tag>
struct ItemModified {
    explicit ItemModified(ItemIterator item) : item{ item } {}

    ItemIterator item;
};

using ItemMovedToFront = ItemModified<struct ItemMovedToFrontTag>;
using ItemUpdated = ItemModified<struct ItemUpdatedTag>;
using ItemHidden = ItemModified<struct ItemHiddenTag>;
using TradabilityUpdated = ItemModified<struct TradabilityUpdatedTag>;

struct ItemEquipped {
    ItemEquipped(ItemIterator item, std::uint8_t slot, csgo::Team team) : item{ item }, slot{ slot }, team{ team } {}

    ItemIterator item;
    std::uint8_t slot;
    csgo::Team team;
};

struct ItemRemoved {
    explicit ItemRemoved(ItemId itemID) : itemID{ itemID } {}

    ItemId itemID;
};

template <typename Tag>
struct StickerModified {
    StickerModified(ItemIterator skinItem, std::uint8_t stickerSlot) : skinItem{ skinItem }, stickerSlot{ stickerSlot } {}

    ItemIterator skinItem;
    std::uint8_t stickerSlot;
};

using StickerApplied = StickerModified<struct StickerAppliedTag>;
using StickerScraped = StickerModified<struct StickerScrapedTag>;
using StickerRemoved = StickerModified<struct StickerRemovedTag>;

struct StatTrakUpdated {
    StatTrakUpdated(ItemIterator item, int newStatTrakValue) : item{ item }, newStatTrakValue{ newStatTrakValue } {}

    ItemIterator item;
    int newStatTrakValue;
};

struct ViewerPassActivated {
    explicit ViewerPassActivated(ItemIterator createdEventCoin) : createdEventCoin{ createdEventCoin } {}

    ItemIterator createdEventCoin;
};

template <typename Tag>
struct NameTagModified {
    explicit NameTagModified(ItemIterator skinItem) : skinItem{ skinItem } {}

    ItemIterator skinItem;
};

using NameTagAdded = NameTagModified<struct NameTagAddedTag>;
using NameTagRemoved = NameTagModified<struct NameTagRemovedTag>;

struct ContainerOpened {
    explicit ContainerOpened(ItemIterator receivedItem) : receivedItem{ receivedItem } {}

    ItemIterator receivedItem;
};

template <typename Tag>
struct PatchModified {
    PatchModified(ItemIterator agentItem, std::uint8_t patchSlot) : agentItem{ agentItem }, patchSlot{ patchSlot } {}

    ItemIterator agentItem;
    std::uint8_t patchSlot;
};

using PatchApplied = PatchModified<struct PatchAppliedTag>;
using PatchRemoved = PatchModified<struct PatchRemovedTag>;

struct SouvenirTokenActivated {
    explicit SouvenirTokenActivated(ItemIterator tournamentCoin) : tournamentCoin{ tournamentCoin } {}

    ItemIterator tournamentCoin;
};

struct GraffitiUnsealed {
    explicit GraffitiUnsealed(ItemIterator graffitiItem) : graffitiItem{ graffitiItem } {}

    ItemIterator graffitiItem;
};

struct StatTrakSwapped {
    explicit StatTrakSwapped(ItemIterator itemWithHigherStatTrakAfterSwap)
        : itemWithHigherStatTrakAfterSwap{ itemWithHigherStatTrakAfterSwap } {}

    ItemIterator itemWithHigherStatTrakAfterSwap;
};

struct TeamGraffitiSelected {
    TeamGraffitiSelected(ItemIterator tournamentCoin, std::uint16_t graffitiID) : tournamentCoin{ tournamentCoin }, graffitiID{ graffitiID } {}

    ItemIterator tournamentCoin;
    std::uint16_t graffitiID;
};

struct PickEmUpdated {};

struct XRayScannerUsed {
    explicit XRayScannerUsed(ItemIterator receivedItem) : receivedItem{ receivedItem } {}

    ItemIterator receivedItem;
};

struct XRayItemClaimed {
    explicit XRayItemClaimed(ItemIterator item) : item{ item } {}

    ItemIterator item;
};

struct StorageUnitNamed {
    explicit StorageUnitNamed(ItemIterator storageUnit) : storageUnit{ storageUnit } {}

    ItemIterator storageUnit;
};

struct StorageUnitModified {
    explicit StorageUnitModified(ItemIterator storageUnit) : storageUnit{ storageUnit } {}

    ItemIterator storageUnit;
};

struct ItemBoundToStorageUnit {
    ItemBoundToStorageUnit(ItemIterator item, ItemIterator storageUnit)
        : item{ item }, storageUnit { storageUnit } {}

    ItemIterator item;
    ItemIterator storageUnit;
};

struct ItemAddedToStorageUnit {
    explicit ItemAddedToStorageUnit(ItemIterator storageUnit) : storageUnit{ storageUnit } {}

    ItemIterator storageUnit;
};

struct ItemRemovedFromStorageUnit {
    ItemRemovedFromStorageUnit(ItemIterator item, ItemIterator storageUnit)
        : item{ item }, storageUnit{ storageUnit } {}

    ItemIterator item;
    ItemIterator storageUnit;
};

struct StorageUnitContentsLoaded {
    explicit StorageUnitContentsLoaded(ItemIterator storageUnit) : storageUnit{ storageUnit } {}

    ItemIterator storageUnit;
};

}
