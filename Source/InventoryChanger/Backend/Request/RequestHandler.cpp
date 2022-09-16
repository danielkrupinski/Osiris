#include <memory>
#include <optional>

#include "RequestHandler.h"

#include <InventoryChanger/Backend/BackendSimulator.h>
#include <InventoryChanger/GameItems/Lookup.h>
#include <InventoryChanger/ItemGenerator/ItemGenerator.h>

namespace inventory_changer::backend
{

void RequestHandler::operator()(const request::ApplySticker& request) const
{
    const auto skin = get<inventory::Skin>(constRemover(request.item));
    if (!skin)
        return;

    skin->stickers[request.slot].stickerID = static_cast<csgo::StickerId>(gameItemLookup.getStorage().getStickerKit(request.sticker->gameItem()).id);
    skin->stickers[request.slot].wear = 0.0f;

    inventoryHandler.moveItemToFront(request.item);

    if (request.item->gameItem().getRarity() > EconRarity::Default) {
        copyTradability(request.sticker, request.item);
    }

    itemRemovalHandler(request.sticker);
    responseAccumulator(response::StickerApplied{ request.item, request.slot });
}

void RequestHandler::operator()(const request::WearSticker& request) const
{
    const auto skin = get<inventory::Skin>(constRemover(request.skin));
    if (!skin)
        return;

    constexpr auto wearStep = 0.12f;
    const auto newWear = (skin->stickers[request.slot].wear += wearStep);

    if (const auto shouldRemove = (newWear >= 1.0f + wearStep)) {
        skin->stickers[request.slot] = {};
        responseAccumulator(response::StickerRemoved{ request.skin, request.slot });
    }

    responseAccumulator(response::StickerScraped{ request.skin, request.slot });
}

void RequestHandler::operator()(const request::SwapStatTrak& request) const
{
    if (!request.statTrakSwapTool->gameItem().isStatTrakSwapTool())
        return;

    const auto statTrakFrom = inventory::getStatTrak(*request.itemFrom);
    if (!statTrakFrom)
        return;

    const auto statTrakTo = inventory::getStatTrak(*request.itemTo);
    if (!statTrakTo)
        return;

    itemRemovalHandler(request.statTrakSwapTool);
    itemModificationHandler.updateStatTrak(request.itemFrom, *statTrakTo);
    itemModificationHandler.updateStatTrak(request.itemTo, *statTrakFrom);
    inventoryHandler.moveItemToFront(request.itemFrom);
    inventoryHandler.moveItemToFront(request.itemTo);
    responseAccumulator(response::ItemUpdated{ *statTrakFrom >= *statTrakTo ? request.itemFrom : request.itemTo });
    responseAccumulator(response::StatTrakSwapped{ *statTrakFrom < *statTrakTo ? request.itemFrom : request.itemTo });
}

void RequestHandler::operator()(const request::ApplyPatch& request) const
{
    const auto agent = constRemover(request.item).getOrCreate<inventory::Agent>();
    if (!agent)
        return;

    agent->patches[request.slot].patchID = gameItemLookup.getStorage().getPatch(request.patch->gameItem()).id;
    inventoryHandler.moveItemToFront(request.item);
    copyTradability(request.patch, request.item);
    itemRemovalHandler(request.patch);
    responseAccumulator(response::PatchApplied{ request.item, request.slot });
}

void RequestHandler::operator()(const request::RemovePatch& request) const
{
    const auto agent = get<inventory::Agent>(constRemover(request.item));
    if (!agent)
        return;

    agent->patches[request.slot].patchID = 0;
    inventoryHandler.moveItemToFront(request.item);
    responseAccumulator(response::PatchRemoved{ request.item, request.slot });
}

void RequestHandler::operator()(const request::AddNameTag& request) const
{
    const auto skin = get<inventory::Skin>(constRemover(request.item));
    if (!skin)
        return;

    skin->nameTag = request.nameTag;
    itemRemovalHandler(request.nameTagItem);
    inventoryHandler.moveItemToFront(request.item);
    responseAccumulator(response::NameTagAdded{ request.item });
}

void RequestHandler::operator()(const request::RemoveNameTag& request) const
{
    if (const auto skin = get<inventory::Skin>(constRemover(request.item))) {
        skin->nameTag.clear();
        inventoryHandler.moveItemToFront(request.item);
        responseAccumulator(response::NameTagRemoved{ request.item });
    }
}

void RequestHandler::operator()(const request::ActivateSouvenirToken& request) const
{
    if (!request.item->gameItem().isSouvenirToken())
        return;

    const auto tournamentEventID = gameItemLookup.getStorage().getTournamentEventID(request.item->gameItem());
    const auto tournamentCoin = std::ranges::find_if(inventory, [this, tournamentEventID](const auto& item) { return item.gameItem().isTournamentCoin() && gameItemLookup.getStorage().getTournamentEventID(item.gameItem()) == tournamentEventID; });
    if (tournamentCoin == inventory.end())
        return;

    const auto tournamentCoinData = constRemover(tournamentCoin).getOrCreate<inventory::TournamentCoin>();
    if (!tournamentCoinData)
        return;

    ++tournamentCoinData->dropsAwarded;
    itemRemovalHandler(request.item);
    responseAccumulator(response::SouvenirTokenActivated{ tournamentCoin });
}

void RequestHandler::operator()(const request::UnsealGraffiti& request) const
{
    if (!request.item->gameItem().isGraffiti())
        return;

    const auto graffiti = constRemover(request.item).getOrCreate<inventory::Graffiti>();
    if (!graffiti)
        return;

    graffiti->usesLeft = 50;

    inventoryHandler.moveItemToFront(request.item);
    responseAccumulator(response::GraffitiUnsealed{ request.item });
}

void RequestHandler::copyTradability(ItemIterator source, ItemIterator destination) const
{
    const auto sourceTradableAfter = source->getProperties().common.tradableAfterDate;
    auto& destinationTradableAfter = constRemover(destination).getProperties().common.tradableAfterDate;

    if (destinationTradableAfter > sourceTradableAfter) {
        destinationTradableAfter = sourceTradableAfter;
        responseAccumulator(response::TradabilityUpdated{ destination });
    }
}

}
