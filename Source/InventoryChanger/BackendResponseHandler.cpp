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
    const auto itemID = game_integration::Inventory{}.createSOCItem(*response.item, response.asUnacknowledged);
    backend.assignItemID(response.item, itemID);
}

void BackendResponseHandler::operator()(const backend::response::ItemMovedToFront& response) const
{
    if (const auto itemID = backend.getItemID(response.item); itemID.has_value())
        backend.updateItemID(*itemID, game_integration::Inventory{}.assingNewItemID(*itemID));
}

void BackendResponseHandler::operator()(const backend::response::ItemUpdated& response) const
{
    if (const auto itemID = backend.getItemID(response.item); itemID.has_value()) {
        game_integration::Inventory{}.markItemUpdated(*itemID);
    }
}

void BackendResponseHandler::operator()(const backend::response::ItemEquipped& response) const
{
    if (const auto itemID = backend.getItemID(response.item); itemID.has_value()) {
        game_integration::Inventory{}.equipItem(*itemID, response.team, response.slot);
    }
}

void BackendResponseHandler::operator()(const backend::response::ItemRemoved& response) const
{
    game_integration::Inventory{}.deleteItem(response.itemID);
}

void BackendResponseHandler::operator()(const backend::response::StickerApplied& response) const
{
    if (const auto itemID = backend.getItemID(response.skinItem); itemID.has_value()) {
        if (const auto skin = response.skinItem->get<inventory::Skin>())
            game_integration::Inventory{}.applySticker(*itemID, skin->stickers[response.stickerSlot].stickerID, response.stickerSlot);
    }
}

void BackendResponseHandler::operator()(const backend::response::StickerScraped& response) const
{
    if (const auto itemID = backend.getItemID(response.skinItem); itemID.has_value()) {
        if (const auto skin = response.skinItem->get<inventory::Skin>())
            game_integration::Inventory{}.updateStickerWear(*itemID, response.stickerSlot, skin->stickers[response.stickerSlot].wear);
    }
}

void BackendResponseHandler::operator()(const backend::response::StickerRemoved& response) const
{
    if (const auto itemID = backend.getItemID(response.skinItem); itemID.has_value()) {
        game_integration::Inventory{}.removeSticker(*itemID, response.stickerSlot);
    }
}

void BackendResponseHandler::operator()(const backend::response::StatTrakUpdated& response) const
{
    game_integration::Inventory{}.updateStatTrak(response.itemID, response.newStatTrakValue);
}

void BackendResponseHandler::operator()(const backend::response::ViewerPassActivated& response) const
{
    if (const auto itemID = backend.getItemID(response.createdEventCoin); itemID.has_value())
        game_integration::Inventory{}.viewerPassActivated(*itemID);
}

void BackendResponseHandler::operator()(const backend::response::NameTagAdded& response) const
{
    if (const auto itemID = backend.getItemID(response.skinItem); itemID.has_value()) {
        if (const auto skin = response.skinItem->get<inventory::Skin>()) {
            game_integration::Inventory{}.addNameTag(*itemID, skin->nameTag.c_str());
        }
    }
}

void BackendResponseHandler::operator()(const backend::response::NameTagRemoved& response) const
{
    if (const auto itemID = backend.getItemID(response.skinItem); itemID.has_value())
        game_integration::Inventory{}.removeNameTag(*itemID);
}

void BackendResponseHandler::operator()(const backend::response::ContainerOpened& response) const
{
    if (const auto itemID = backend.getItemID(response.receivedItem); itemID.has_value())
        game_integration::Inventory{}.containerOpened(*itemID);
}

void BackendResponseHandler::operator()(const backend::response::PatchApplied& response) const
{
    if (const auto itemID = backend.getItemID(response.agentItem); itemID.has_value()) {
        if (const auto agent = response.agentItem->get<inventory::Agent>()) {
            game_integration::Inventory{}.applyPatch(*itemID, agent->patches[response.patchSlot].patchID, response.patchSlot);
        }
    }
}

void BackendResponseHandler::operator()(const backend::response::PatchRemoved& response) const
{
    if (const auto itemID = backend.getItemID(response.agentItem); itemID.has_value()) {
        game_integration::Inventory{}.removePatch(*itemID, response.patchSlot);
    }
}

void BackendResponseHandler::operator()(const backend::response::SouvenirTokenActivated& response) const
{
    if (const auto itemID = backend.getItemID(response.tournamentCoin); itemID.has_value()) {
        if (const auto tournamentCoin = response.tournamentCoin->get<inventory::TournamentCoin>()) {
            game_integration::Inventory{}.souvenirTokenActivated(*itemID, tournamentCoin->dropsAwarded);
        }
    }
}

void BackendResponseHandler::operator()(const backend::response::GraffitiUnsealed& response) const
{
    if (const auto itemID = backend.getItemID(response.graffitiItem); itemID.has_value()) {
        game_integration::Inventory{}.unsealGraffiti(*itemID);
    }
}

void BackendResponseHandler::operator()(const backend::response::StatTrakSwapped& response) const
{
    const auto sourceItemID = backend.getItemID(response.swapSourceItem);
    if (!sourceItemID.has_value())
        return;

    const auto destinationItemID = backend.getItemID(response.swapDestinationItem);
    if (!destinationItemID.has_value())
        return;

    const auto sourceStatTrak = inventory::getStatTrak(*response.swapSourceItem);
    if (!sourceStatTrak)
        return;

    const auto destinationStatTrak = inventory::getStatTrak(*response.swapDestinationItem);
    if (!destinationStatTrak)
        return;

    game_integration::Inventory{}.statTrakSwapped(*sourceStatTrak > *destinationStatTrak ? *sourceItemID : *destinationItemID);
}

void BackendResponseHandler::operator()(const backend::response::TeamGraffitiSelected& response) const
{
    if (const auto itemID = backend.getItemID(response.tournamentCoin); itemID.has_value()) {
        game_integration::Inventory{}.selectTeamGraffiti(*itemID, response.graffitiID);
    }
}

}
