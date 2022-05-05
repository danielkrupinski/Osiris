#pragma once

#include <variant>

#include <InventoryChanger/Backend/Item.h>

#include "Response.h"
#include "ResponseTypes.h"

namespace inventory_changer::backend
{

struct ItemInResponse {
    explicit ItemInResponse(ItemConstIterator item) : item{ item } {}

    bool operator()(std::monostate) const { return false; }
    bool operator()(const response::ItemAdded& response) const { return response.item == item; }
    bool operator()(const response::ItemMovedToFront& response) const { return response.item == item; }
    bool operator()(const response::ItemUpdated& response) const { return response.item == item; }
    bool operator()(const response::ItemEquipped& response) const { return response.item == item; }
    bool operator()(const response::ItemRemoved&) const { return false; }
    bool operator()(const response::StickerApplied& response) const { return response.skinItem == item; }
    bool operator()(const response::StickerScraped& response) const { return response.skinItem == item; }
    bool operator()(const response::StickerRemoved& response) const { return response.skinItem == item; }
    bool operator()(const response::StatTrakUpdated&) const { return false; }
    bool operator()(const response::ViewerPassActivated& response) const { return response.createdEventCoin == item; }
    bool operator()(const response::NameTagAdded& response) const { return response.skinItem == item; }
    bool operator()(const response::NameTagRemoved& response) const { return response.skinItem == item; }
    bool operator()(const response::ContainerOpened& response) const { return response.receivedItem == item; }
    bool operator()(const response::PatchApplied& response) const { return response.agentItem == item; }
    bool operator()(const response::PatchRemoved& response) const { return response.agentItem == item; }
    bool operator()(const response::SouvenirTokenActivated& response) const { return response.tournamentCoin == item; }
    bool operator()(const response::GraffitiUnsealed& response) const { return response.graffitiItem == item; }
    bool operator()(const response::StatTrakSwapped& response) const { return response.swapDestinationItem == item || response.swapSourceItem == item; }
    bool operator()(const response::TeamGraffitiSelected& response) const { return response.tournamentCoin == item; }

private:
    ItemConstIterator item;
};

inline bool responseContainsItem(const Response& response, ItemConstIterator item)
{
    return std::visit(ItemInResponse{ item }, response);
}

}
