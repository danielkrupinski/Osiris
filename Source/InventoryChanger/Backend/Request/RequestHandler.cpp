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

    skin->stickers[request.slot].stickerID = gameItemLookup.getStorage().getStickerKit(request.sticker->gameItem()).id;
    skin->stickers[request.slot].wear = 0.0f;

    inventoryHandler.moveItemToFront(request.item);
    itemRemovalHandler.removeItem(request.sticker);
    responseQueue.add(response::StickerApplied{ request.item, request.slot });
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
        responseQueue.add(response::StickerRemoved{ request.skin, request.slot });
    }

    responseQueue.add(response::StickerScraped{ request.skin, request.slot });
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

    itemRemovalHandler.removeItem(request.statTrakSwapTool);
    itemModificationHandler.updateStatTrak(request.itemFrom, *statTrakTo);
    itemModificationHandler.updateStatTrak(request.itemTo, *statTrakFrom);
    inventoryHandler.moveItemToFront(request.itemFrom);
    inventoryHandler.moveItemToFront(request.itemTo);
    responseQueue.add(response::ItemUpdated{ *statTrakFrom >= *statTrakTo ? request.itemFrom : request.itemTo });
    responseQueue.add(response::StatTrakSwapped{ *statTrakFrom < *statTrakTo ? request.itemFrom : request.itemTo });
}

[[nodiscard]] const inventory::Item* toPointer(const std::optional<ItemIterator>& item)
{
    if (item.has_value())
        return std::to_address(*item);
    else
        return nullptr;
}

void RequestHandler::operator()(const request::OpenContainer& request) const
{
    if (!request.container->gameItem().isCrate())
        return;

    auto generatedItem = item_generator::generateItemFromContainer(gameItemLookup, crateLootLookup, *request.container, toPointer(request.key));
    if (!generatedItem.has_value())
        return;

    if (request.key.has_value()) {
        if (const auto& keyItem = *request.key; keyItem->gameItem().isCaseKey())
            itemRemovalHandler.removeItem(keyItem);
    }

    itemRemovalHandler.removeItem(request.container);
    const auto receivedItem = inventoryHandler.addItem(std::move(*generatedItem), true);
    responseQueue.add(response::ContainerOpened{ receivedItem });
}

void RequestHandler::operator()(const request::ApplyPatch& request) const
{
    const auto agent = constRemover(request.item).getOrCreate<inventory::Agent>();
    if (!agent)
        return;

    agent->patches[request.slot].patchID = gameItemLookup.getStorage().getPatch(request.patch->gameItem()).id;
    inventoryHandler.moveItemToFront(request.item);
    itemRemovalHandler.removeItem(request.patch);
    responseQueue.add(response::PatchApplied{ request.item, request.slot });
}

void RequestHandler::operator()(const request::RemovePatch& request) const
{
    const auto agent = get<inventory::Agent>(constRemover(request.item));
    if (!agent)
        return;

    agent->patches[request.slot].patchID = 0;
    inventoryHandler.moveItemToFront(request.item);
    responseQueue.add(response::PatchRemoved{ request.item, request.slot });
}

void RequestHandler::operator()(const request::ActivateOperationPass& request) const
{
    const auto& gameItem = request.operationPass->gameItem();
    if (!gameItem.isOperationPass())
        return;

    const auto coinID = gameItem.getWeaponID() != WeaponId::OperationHydraPass ? static_cast<WeaponId>(static_cast<int>(gameItem.getWeaponID()) + 1) : WeaponId::BronzeOperationHydraCoin;
    if (const auto operationCoin = gameItemLookup.findItem(coinID)) {
        inventoryHandler.addItem(inventory::Item{ *operationCoin }, true);
        itemRemovalHandler.removeItem(request.operationPass);
    }
}

void RequestHandler::operator()(const request::ActivateViewerPass& request) const
{
    const auto& gameItem = request.item->gameItem();
    if (!gameItem.isViewerPass())
        return;

    const auto coinID = Helpers::bronzeEventCoinFromViewerPass(gameItem.getWeaponID());
    if (coinID == WeaponId::None)
        return;

    if (const auto eventCoin = gameItemLookup.findItem(coinID)) {
        const auto addedEventCoin = inventoryHandler.addItem(inventory::Item{ *eventCoin, inventory::TournamentCoin{ Helpers::numberOfTokensWithViewerPass(gameItem.getWeaponID()) }, }, true);
        itemRemovalHandler.removeItem(request.item);
        responseQueue.add(response::ViewerPassActivated{ addedEventCoin });
    }
}

void RequestHandler::operator()(const request::AddNameTag& request) const
{
    const auto skin = get<inventory::Skin>(constRemover(request.item));
    if (!skin)
        return;

    skin->nameTag = request.nameTag;
    itemRemovalHandler.removeItem(request.nameTagItem);
    inventoryHandler.moveItemToFront(request.item);
    responseQueue.add(response::NameTagAdded{ request.item });
}

void RequestHandler::operator()(const request::RemoveNameTag& request) const
{
    if (const auto skin = get<inventory::Skin>(constRemover(request.item))) {
        skin->nameTag.clear();
        inventoryHandler.moveItemToFront(request.item);
        responseQueue.add(response::NameTagRemoved{ request.item });
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
    itemRemovalHandler.removeItem(request.item);
    responseQueue.add(response::SouvenirTokenActivated{ tournamentCoin });
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
    responseQueue.add(response::GraffitiUnsealed{ request.item });
}

}
