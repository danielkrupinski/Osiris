#include "BackendResponseHandler.h"

#include "StaticData.h"
#include "GameItems/Lookup.h"
#include <SDK/EconItemView.h>
#include <SDK/Panorama.h>
#include <Interfaces.h>

#include "GameIntegration/Inventory.h"

namespace inventory_changer
{

void BackendResponseHandler::operator()(const backend::response::ItemAdded& response) const
{
    const auto itemID = gameInventory.createSOCItem(*response.item, response.asUnacknowledged);
    backend.assignItemID(response.item, itemID);
}

void BackendResponseHandler::operator()(const backend::response::ItemMovedToFront& response) const
{
    if (const auto itemID = backend.getItemID(response.item); itemID.has_value())
        backend.updateItemID(*itemID, gameInventory.assingNewItemID(*itemID));
}

void BackendResponseHandler::operator()(const backend::response::ItemUpdated& response) const
{
    if (const auto itemID = backend.getItemID(response.item); itemID.has_value()) {
        gameInventory.markItemUpdated(*itemID);
    }
}

void BackendResponseHandler::operator()(const backend::response::ItemEquipped& response) const
{
    if (const auto itemID = backend.getItemID(response.item); itemID.has_value()) {
        gameInventory.equipItem(*itemID, response.team, response.slot);
    }
}

void BackendResponseHandler::operator()(const backend::response::ItemRemoved& response) const
{
    gameInventory.deleteItem(response.itemID);
}

void BackendResponseHandler::operator()(const backend::response::StickerApplied& response) const
{
    if (const auto itemID = backend.getItemID(response.skinItem); itemID.has_value()) {
        if (const auto skin = response.skinItem->get<inventory::Skin>())
            gameInventory.applySticker(*itemID, skin->stickers[response.stickerSlot].stickerID, response.stickerSlot);
    }
}

void BackendResponseHandler::operator()(const backend::response::StickerScraped& response) const
{
    if (const auto itemID = backend.getItemID(response.skinItem); itemID.has_value()) {
        if (const auto skin = response.skinItem->get<inventory::Skin>())
            gameInventory.updateStickerWear(*itemID, response.stickerSlot, skin->stickers[response.stickerSlot].wear);
    }
}

void BackendResponseHandler::operator()(const backend::response::StickerRemoved& response) const
{
    if (const auto itemID = backend.getItemID(response.skinItem); itemID.has_value()) {
        gameInventory.removeSticker(*itemID, response.stickerSlot);
    }
}

void BackendResponseHandler::operator()(const backend::response::StatTrakUpdated& response) const
{
    gameInventory.updateStatTrak(response.itemID, response.newStatTrakValue);
}

void BackendResponseHandler::operator()(const backend::response::ViewerPassActivated& response) const
{
    if (const auto itemID = backend.getItemID(response.createdEventCoin); itemID.has_value())
        gameInventory.viewerPassActivated(*itemID);
}

void BackendResponseHandler::operator()(const backend::response::NameTagAdded& response) const
{
    if (const auto itemID = backend.getItemID(response.skinItem); itemID.has_value()) {
        if (const auto skin = response.skinItem->get<inventory::Skin>()) {
            gameInventory.addNameTag(*itemID, skin->nameTag.c_str());
        }
    }
}

void BackendResponseHandler::operator()(const backend::response::NameTagRemoved& response) const
{
    if (const auto itemID = backend.getItemID(response.skinItem); itemID.has_value())
        gameInventory.removeNameTag(*itemID);
}

void BackendResponseHandler::operator()(const backend::response::ContainerOpened& response) const
{
    if (const auto itemID = backend.getItemID(response.receivedItem); itemID.has_value())
        gameInventory.containerOpened(*itemID);
}

void BackendResponseHandler::operator()(const backend::response::PatchApplied& response) const
{
    if (const auto itemID = backend.getItemID(response.agentItem); itemID.has_value()) {
        if (const auto agent = response.agentItem->get<inventory::Agent>()) {
            gameInventory.applyPatch(*itemID, agent->patches[response.patchSlot].patchID, response.patchSlot);
        }
    }
}

void BackendResponseHandler::operator()(const backend::response::PatchRemoved& response) const
{
    if (const auto itemID = backend.getItemID(response.agentItem); itemID.has_value()) {
        gameInventory.removePatch(*itemID, response.patchSlot);
    }
}

void BackendResponseHandler::operator()(const backend::response::SouvenirTokenActivated& response) const
{
    if (const auto itemID = backend.getItemID(response.tournamentCoin); itemID.has_value()) {
        if (const auto tournamentCoin = response.tournamentCoin->get<inventory::TournamentCoin>()) {
            gameInventory.souvenirTokenActivated(*itemID, tournamentCoin->dropsAwarded);
        }
    }
}

void BackendResponseHandler::operator()(const backend::response::GraffitiUnsealed& response) const
{
    if (const auto itemID = backend.getItemID(response.graffitiItem); itemID.has_value()) {
        gameInventory.unsealGraffiti(*itemID);
    }
}

void BackendResponseHandler::operator()(const backend::response::StatTrakSwapped& response) const
{
    if (const auto itemID = backend.getItemID(response.itemWithHigherStatTrakAfterSwap); itemID.has_value()) {
        gameInventory.statTrakSwapped(*itemID);
    }
}

void BackendResponseHandler::operator()(const backend::response::TeamGraffitiSelected& response) const
{
    if (const auto itemID = backend.getItemID(response.tournamentCoin); itemID.has_value()) {
        gameInventory.selectTeamGraffiti(*itemID, response.graffitiID);
    }
}

}
