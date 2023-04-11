#pragma once

#include <variant>

#include <InventoryChanger/Backend/Item.h>

#include "Response.h"
#include "ResponseTypes.h"

namespace inventory_changer::backend
{

struct ItemInResponse {
    explicit ItemInResponse(ItemIterator item) : item{ item } {}

    template <typename Tag>
    bool operator()(const response::ItemModified<Tag>& response) const { return response.item == item; }

    template <typename Tag>
    bool operator()(const response::NameTagModified<Tag>& response) const { return response.skinItem == item; }

    template <typename Tag>
    bool operator()(const response::PatchModified<Tag>& response) const { return response.agentItem == item; }

    template <typename Tag>
    bool operator()(const response::StickerModified<Tag>& response) const { return response.skinItem == item; }

    bool operator()(const response::ItemRemoved&) const { return false; }
    bool operator()(const response::PickEmUpdated&) const { return false; }

    bool operator()(const response::ContainerOpened& response) const { return response.receivedItem == item; }
    bool operator()(const response::GraffitiUnsealed& response) const { return response.graffitiItem == item; }
    bool operator()(const response::ItemAdded& response) const { return response.item == item; }
    bool operator()(const response::ItemEquipped& response) const { return response.item == item; }
    bool operator()(const response::SouvenirTokenActivated& response) const { return response.tournamentCoin == item; }
    bool operator()(const response::StatTrakSwapped& response) const { return response.itemWithHigherStatTrakAfterSwap == item; }
    bool operator()(const response::StatTrakUpdated& response) const { return response.item == item; }
    bool operator()(const response::TeamGraffitiSelected& response) const { return response.tournamentCoin == item; }
    bool operator()(const response::ViewerPassActivated& response) const { return response.createdEventCoin == item; }
    bool operator()(const response::XRayItemClaimed& response) const { return response.item == item; }
    bool operator()(const response::XRayScannerUsed& response) const { return response.receivedItem == item; }
    bool operator()(const response::StorageUnitNamed& response) const { return response.storageUnit == item; }
    bool operator()(const response::StorageUnitModified& response) const { return response.storageUnit == item; }
    bool operator()(const response::ItemBoundToStorageUnit& response) const { return response.item == item || response.storageUnit == item; }
    bool operator()(const response::ItemAddedToStorageUnit& response) const { return response.storageUnit == item; }
    bool operator()(const response::ItemRemovedFromStorageUnit& response) const { return response.item == item || response.storageUnit == item; }
    bool operator()(const response::StorageUnitContentsLoaded& response) const { return response.storageUnit == item; }

private:
    ItemIterator item;
};

inline bool responseContainsItem(const Response& response, ItemIterator item)
{
    return std::visit(ItemInResponse{ item }, response);
}

}
