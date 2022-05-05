#include "BackendSimulator.h"
#include "RequestHandler.h"

#include <InventoryChanger/GameItems/Lookup.h>
#include <InventoryChanger/ItemGenerator.h>

namespace inventory_changer::backend
{

Response RequestHandler::operator()(const request::ApplySticker& request) const
{
    const auto skin = constRemover.removeConstness(request.item)->get<inventory::Skin>();
    if (!skin)
        return {};

    skin->stickers[request.slot].stickerID = gameItemLookup.getStorage().getStickerKit(request.sticker->gameItem()).id;
    skin->stickers[request.slot].wear = 0.0f;

    backend.moveToFront(request.item);
    backend.removeItem(request.sticker);
    return response::StickerApplied{ request.item, request.slot };
}

Response RequestHandler::operator()(const request::WearSticker& request) const
{
    const auto skin = constRemover.removeConstness(request.skin)->get<inventory::Skin>();
    if (!skin)
        return {};

    constexpr auto wearStep = 0.12f;
    const auto newWear = (skin->stickers[request.slot].wear += wearStep);

    if (const auto shouldRemove = (newWear >= 1.0f + wearStep)) {
        skin->stickers[request.slot] = {};
        return response::StickerRemoved{ request.skin, request.slot };
    }

    return response::StickerScraped{ request.skin, request.slot };
}

Response RequestHandler::operator()(const request::SwapStatTrak& request) const
{
    if (!request.statTrakSwapTool->gameItem().isStatTrakSwapTool())
        return {};

    const auto statTrakFrom = inventory::getStatTrak(*constRemover.removeConstness(request.itemFrom));
    if (!statTrakFrom)
        return {};

    const auto statTrakTo = inventory::getStatTrak(*constRemover.removeConstness(request.itemTo));
    if (!statTrakTo)
        return {};

    std::swap(*statTrakFrom, *statTrakTo);
    backend.removeItem(request.statTrakSwapTool);
    backend.moveToFront(request.itemFrom);
    backend.moveToFront(request.itemTo);
    return response::StatTrakSwapped{ request.itemFrom, request.itemTo };
}

Response RequestHandler::operator()(const request::OpenContainer& request) const
{
    if (!request.container->gameItem().isCase())
        return {};

    if (request.key.has_value()) {
        if (const auto& keyItem = *request.key; keyItem->gameItem().isCaseKey())
            backend.removeItem(keyItem);
    }

    auto generatedItem = ItemGenerator::generateItemFromContainer(*request.container);
    backend.removeItem(request.container);
    const auto receivedItem = backend.addItemUnacknowledged(std::move(generatedItem));
    return response::ContainerOpened{ receivedItem };
}

Response RequestHandler::operator()(const request::ApplyPatch& request) const
{
    const auto agent = constRemover.removeConstness(request.item)->getOrCreate<inventory::Agent>();
    if (!agent)
        return {};

    agent->patches[request.slot].patchID = gameItemLookup.getStorage().getPatch(request.patch->gameItem()).id;
    backend.moveToFront(request.item);
    backend.removeItem(request.patch);
    return response::PatchApplied{ request.item, request.slot };
}

Response RequestHandler::operator()(const request::RemovePatch& request) const
{
    const auto agent = constRemover.removeConstness(request.item)->get<inventory::Agent>();
    if (!agent)
        return {};

    agent->patches[request.slot].patchID = 0;
    backend.moveToFront(request.item);
    return response::PatchRemoved{ request.item, request.slot };
}

Response RequestHandler::operator()(const request::ActivateOperationPass& request) const
{
    const auto& gameItem = request.operationPass->gameItem();
    if (!gameItem.isOperationPass())
        return {};

    const auto coinID = gameItem.getWeaponID() != WeaponId::OperationHydraPass ? static_cast<WeaponId>(static_cast<int>(gameItem.getWeaponID()) + 1) : WeaponId::BronzeOperationHydraCoin;
    if (const auto operationCoin = gameItemLookup.findItem(coinID); operationCoin.has_value()) {
        backend.addItemUnacknowledged(inventory::Item{ *operationCoin });
        backend.removeItem(request.operationPass);
    }
    return {};
}

Response RequestHandler::operator()(const request::ActivateViewerPass& request) const
{
    const auto& gameItem = request.viewerPass->gameItem();
    if (!gameItem.isViewerPass())
        return {};

    const auto coinID = Helpers::bronzeEventCoinFromViewerPass(gameItem.getWeaponID());
    if (coinID == WeaponId::None)
        return {};

    if (const auto eventCoin = gameItemLookup.findItem(coinID); eventCoin.has_value()) {
        const auto addedEventCoin = backend.addItemUnacknowledged(inventory::Item{ *eventCoin, inventory::TournamentCoin{ Helpers::numberOfTokensWithViewerPass(gameItem.getWeaponID()) } });
        backend.removeItem(request.viewerPass);
        return response::ViewerPassActivated{ addedEventCoin };
    }
    return {};
}

Response RequestHandler::operator()(const request::AddNameTag& request) const
{
    const auto skin = constRemover.removeConstness(request.item)->get<inventory::Skin>();
    if (!skin)
        return {};

    skin->nameTag = request.nameTag;
    backend.removeItem(request.nameTagItem);
    backend.moveToFront(request.item);
    return response::NameTagAdded{ request.item };
}

Response RequestHandler::operator()(const request::RemoveNameTag& request) const
{
    if (const auto skin = constRemover.removeConstness(request.item)->get<inventory::Skin>()) {
        skin->nameTag.clear();
        backend.moveToFront(request.item);
        return response::NameTagRemoved{ request.item };
    }
    return {};
}

Response RequestHandler::operator()(const request::ActivateSouvenirToken& request) const
{
    if (!request.souvenirToken->gameItem().isSouvenirToken())
        return {};

    const auto tournamentEventID = gameItemLookup.getStorage().getTournamentEventID(request.souvenirToken->gameItem());
    const auto& inventory = backend.getInventory();
    const auto tournamentCoin = std::ranges::find_if(inventory, [this, tournamentEventID](const auto& item) { return item.gameItem().isTournamentCoin() && gameItemLookup.getStorage().getTournamentEventID(item.gameItem()) == tournamentEventID; });
    if (tournamentCoin == inventory.end())
        return {};

    const auto tournamentCoinData = constRemover.removeConstness(tournamentCoin)->getOrCreate<inventory::TournamentCoin>();
    if (!tournamentCoinData)
        return {};

    ++tournamentCoinData->dropsAwarded;
    backend.removeItem(request.souvenirToken);
    return response::SouvenirTokenActivated{ tournamentCoin };
}

Response RequestHandler::operator()(const request::UnsealGraffiti& request) const
{
    if (!request.item->gameItem().isGraffiti())
        return {};

    const auto graffiti = constRemover.removeConstness(request.item)->getOrCreate<inventory::Graffiti>();
    if (!graffiti)
        return {};

    graffiti->usesLeft = 50;

    backend.moveToFront(request.item);
    return response::GraffitiUnsealed{ request.item };
}

Response RequestHandler::operator()(const request::UpdateStatTrak& request) const
{
    const auto statTrak = inventory::getStatTrak(*constRemover.removeConstness(request.item));
    if (!statTrak)
        return {};

    *statTrak = request.newStatTrak;
    if (const auto itemID = backend.getItemID(request.item); itemID.has_value())
        return response::StatTrakUpdated{ *itemID, request.newStatTrak };
    return {};
}

Response RequestHandler::operator()(const request::SelectTeamGraffiti& request) const
{
    if (request.tournamentCoin->gameItem().isTournamentCoin())
        return response::TeamGraffitiSelected{ request.tournamentCoin, request.graffitiID };
    return {};
}

}
