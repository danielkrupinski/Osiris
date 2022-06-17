#pragma once

#include <variant>

#include <InventoryChanger/Backend/ItemIDMap.h>
#include <InventoryChanger/GameItems/Storage.h>

#include "ResponseTypes.h"

namespace inventory_changer::backend
{

template <typename GameInventory>
struct ResponseHandler {
    explicit ResponseHandler(const game_items::Storage& gameItemStorage, ItemIDMap& itemIDMap, GameInventory& gameInventory)
        : gameItemStorage{ gameItemStorage }, itemIDMap{ itemIDMap }, gameInventory{ gameInventory } {}

    void operator()(std::monostate) const { /* Empty response, this should never be called */ }

    void operator()(const response::ItemAdded& response) const
    {
        const auto itemID = gameInventory.createSOCItem(gameItemStorage, *response.item, response.asUnacknowledged);
        itemIDMap.add(itemID, response.item);
    }

    void operator()(const response::ItemMovedToFront& response) const
    {
        if (const auto itemID = getItemID(response.item); itemID.has_value())
            itemIDMap.update(*itemID, gameInventory.assingNewItemID(*itemID));
    }

    void operator()(const response::ItemUpdated& response) const
    {
        if (const auto itemID = getItemID(response.item); itemID.has_value())
            gameInventory.markItemUpdated(*itemID);
    }

    void operator()(const response::ItemHidden& response) const
    {
        if (const auto itemID = getItemID(response.item); itemID.has_value())
            gameInventory.hideItem(*itemID);
    }

    void operator()(const response::ItemUnhidden& response) const
    {
        if (const auto itemID = getItemID(response.item); itemID.has_value())
            gameInventory.unhideItem(*itemID);
    }

    void operator()(const response::ItemEquipped& response) const
    {
        if (const auto itemID = getItemID(response.item); itemID.has_value())
            gameInventory.equipItem(*itemID, response.team, response.slot);
    }

    void operator()(const response::ItemRemoved& response) const
    {
        gameInventory.deleteItem(response.itemID);
    }

    void operator()(const response::StickerApplied& response) const
    {
        if (const auto itemID = getItemID(response.skinItem); itemID.has_value()) {
            if (const auto skin = response.skinItem->get<inventory::Skin>())
                gameInventory.applySticker(*itemID, skin->stickers[response.stickerSlot].stickerID, response.stickerSlot);
        }
    }

    void operator()(const response::StickerScraped& response) const
    {
        if (const auto itemID = getItemID(response.skinItem); itemID.has_value()) {
            if (const auto skin = response.skinItem->get<inventory::Skin>())
                gameInventory.updateStickerWear(*itemID, response.stickerSlot, skin->stickers[response.stickerSlot].wear);
        }
    }

    void operator()(const response::StickerRemoved& response) const
    {
        if (const auto itemID = getItemID(response.skinItem); itemID.has_value())
            gameInventory.removeSticker(*itemID, response.stickerSlot);
    }

    void operator()(const response::StatTrakUpdated& response) const
    {
        if (const auto itemID = getItemID(response.item); itemID.has_value())
            gameInventory.updateStatTrak(*itemID, response.newStatTrakValue);
    }

    void operator()(const response::ViewerPassActivated& response) const
    {
        if (const auto itemID = getItemID(response.createdEventCoin); itemID.has_value())
            gameInventory.viewerPassActivated(*itemID);
    }

    void operator()(const response::NameTagAdded& response) const
    {
        if (const auto itemID = getItemID(response.skinItem); itemID.has_value()) {
            if (const auto skin = response.skinItem->get<inventory::Skin>())
                gameInventory.addNameTag(*itemID, skin->nameTag.c_str());
        }
    }

    void operator()(const response::NameTagRemoved& response) const
    {
        if (const auto itemID = getItemID(response.skinItem); itemID.has_value())
            gameInventory.removeNameTag(*itemID);
    }

    void operator()(const response::ContainerOpened& response) const
    {
        if (const auto itemID = getItemID(response.receivedItem); itemID.has_value())
            gameInventory.containerOpened(*itemID);
    }

    void operator()(const response::PatchApplied& response) const
    {
        if (const auto itemID = getItemID(response.agentItem); itemID.has_value()) {
            if (const auto agent = response.agentItem->get<inventory::Agent>())
                gameInventory.applyPatch(*itemID, agent->patches[response.patchSlot].patchID, response.patchSlot);
        }
    }

    void operator()(const response::PatchRemoved& response) const
    {
        if (const auto itemID = getItemID(response.agentItem); itemID.has_value())
            gameInventory.removePatch(*itemID, response.patchSlot);
    }

    void operator()(const response::SouvenirTokenActivated& response) const
    {
        if (const auto itemID = getItemID(response.tournamentCoin); itemID.has_value()) {
            if (const auto tournamentCoin = response.tournamentCoin->get<inventory::TournamentCoin>())
                gameInventory.souvenirTokenActivated(*itemID, tournamentCoin->dropsAwarded);
        }
    }

    void operator()(const response::GraffitiUnsealed& response) const
    {
        if (const auto itemID = getItemID(response.graffitiItem); itemID.has_value())
            gameInventory.unsealGraffiti(*itemID);
    }

    void operator()(const response::StatTrakSwapped& response) const
    {
        if (const auto itemID = getItemID(response.itemWithHigherStatTrakAfterSwap); itemID.has_value())
            gameInventory.statTrakSwapped(*itemID);
    }

    void operator()(const response::TeamGraffitiSelected& response) const
    {
        if (const auto itemID = getItemID(response.tournamentCoin); itemID.has_value())
            gameInventory.selectTeamGraffiti(*itemID, response.graffitiID);
    }

    void operator()(const response::PickEmUpdated&) const
    {
        gameInventory.pickEmUpdated();
    }

    void operator()(const response::XRayScannerUsed& response) const
    {
        if (const auto itemID = getItemID(response.receivedItem); itemID.has_value())
            gameInventory.xRayItemRevealed(*itemID);
    }

    void operator()(const response::XRayItemClaimed& response) const
    {
        if (const auto itemID = getItemID(response.item); itemID.has_value())
            gameInventory.xRayItemClaimed(*itemID);
    }

private:
    [[nodiscard]] auto getItemID(ItemIterator item) const
    {
        return itemIDMap.getItemID(item);
    }

    const game_items::Storage& gameItemStorage;
    ItemIDMap& itemIDMap;
    GameInventory& gameInventory;
};

}
