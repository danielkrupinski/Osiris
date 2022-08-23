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
    const auto skin = get<inventory::Skin>(*constRemover.removeConstness(request.item));
    if (!skin)
        return;

    skin->stickers[request.slot].stickerID = gameItemLookup.getStorage().getStickerKit(request.sticker->gameItem()).id;
    skin->stickers[request.slot].wear = 0.0f;

    inventoryHandler.moveItemToFront(request.item);
    operator()(request::RemoveItem{ request.sticker });
    responseQueue.add(response::StickerApplied{ request.item, request.slot });
}

void RequestHandler::operator()(const request::WearSticker& request) const
{
    const auto skin = get<inventory::Skin>(*constRemover.removeConstness(request.skin));
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

    operator()(request::RemoveItem{ request.statTrakSwapTool });
    operator()(request::UpdateStatTrak{ request.itemFrom, *statTrakTo });
    operator()(request::UpdateStatTrak{ request.itemTo, *statTrakFrom });
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
            operator()(request::RemoveItem{ keyItem });
    }

    operator()(request::RemoveItem{ request.container });
    const auto receivedItem = inventoryHandler.addItem(std::move(*generatedItem), true);
    responseQueue.add(response::ContainerOpened{ receivedItem });
}

void RequestHandler::operator()(const request::ApplyPatch& request) const
{
    const auto agent = constRemover.removeConstness(request.item)->getOrCreate<inventory::Agent>();
    if (!agent)
        return;

    agent->patches[request.slot].patchID = gameItemLookup.getStorage().getPatch(request.patch->gameItem()).id;
    inventoryHandler.moveItemToFront(request.item);
    operator()(request::RemoveItem{ request.patch });
    responseQueue.add(response::PatchApplied{ request.item, request.slot });
}

void RequestHandler::operator()(const request::RemovePatch& request) const
{
    const auto agent = get<inventory::Agent>(*constRemover.removeConstness(request.item));
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
        operator()(request::RemoveItem{ request.operationPass });
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
        operator()(request::RemoveItem{ request.item });
        responseQueue.add(response::ViewerPassActivated{ addedEventCoin });
    }
}

void RequestHandler::operator()(const request::AddNameTag& request) const
{
    const auto skin = get<inventory::Skin>(*constRemover.removeConstness(request.item));
    if (!skin)
        return;

    skin->nameTag = request.nameTag;
    operator()(request::RemoveItem{ request.nameTagItem });
    inventoryHandler.moveItemToFront(request.item);
    responseQueue.add(response::NameTagAdded{ request.item });
}

void RequestHandler::operator()(const request::RemoveNameTag& request) const
{
    if (const auto skin = get<inventory::Skin>(*constRemover.removeConstness(request.item))) {
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

    const auto tournamentCoinData = constRemover.removeConstness(tournamentCoin)->getOrCreate<inventory::TournamentCoin>();
    if (!tournamentCoinData)
        return;

    ++tournamentCoinData->dropsAwarded;
    operator()(request::RemoveItem{ request.item });
    responseQueue.add(response::SouvenirTokenActivated{ tournamentCoin });
}

void RequestHandler::operator()(const request::UnsealGraffiti& request) const
{
    if (!request.item->gameItem().isGraffiti())
        return;

    const auto graffiti = constRemover.removeConstness(request.item)->getOrCreate<inventory::Graffiti>();
    if (!graffiti)
        return;

    graffiti->usesLeft = 50;

    inventoryHandler.moveItemToFront(request.item);
    responseQueue.add(response::GraffitiUnsealed{ request.item });
}

void RequestHandler::operator()(const request::UpdateStatTrak& request) const
{
    const auto statTrak = inventory::getStatTrak(*constRemover.removeConstness(request.item));
    if (!statTrak)
        return;

    *statTrak = request.newStatTrak;
    responseQueue.add(response::StatTrakUpdated{ request.item, request.newStatTrak });
}

void RequestHandler::operator()(const request::SelectTeamGraffiti& request) const
{
    if (request.tournamentCoin->gameItem().isTournamentCoin())
        responseQueue.add(response::TeamGraffitiSelected{ request.tournamentCoin, request.graffitiID });
}

void RequestHandler::operator()(const request::PerformXRayScan& request) const
{
    if (!request.crate->gameItem().isCrate())
        return;

    auto generatedItem = item_generator::generateItemFromContainer(gameItemLookup, crateLootLookup, *request.crate, nullptr);
    if (!generatedItem.has_value())
        return;

    constRemover.removeConstness(request.crate)->setState(inventory::Item::State::InXrayScanner);
    responseQueue.add(response::ItemHidden{ request.crate });

    generatedItem->setState(inventory::Item::State::InXrayScanner);

    const auto receivedItem = inventoryHandler.addItem(std::move(*generatedItem), true);
    xRayScanner.storeItems(XRayScanner::Items{ receivedItem, request.crate });
    responseQueue.add(response::XRayScannerUsed{ receivedItem });
}

void RequestHandler::operator()(const request::ClaimXRayScannedItem& request) const
{
    const auto scannerItems = xRayScanner.getItems();
    if (!scannerItems.has_value())
        return;

    if (request.container != scannerItems->crate)
        return;

    if (request.key.has_value()) {
        if (const auto& keyItem = *request.key; keyItem->gameItem().isCaseKey()) {
            constRemover.removeConstness(scannerItems->reward)->getProperties().common.tradableAfterDate = keyItem->getProperties().common.tradableAfterDate;
            operator()(request::RemoveItem{ keyItem });
        }
    }

    operator()(request::RemoveItem{ request.container });
    constRemover.removeConstness(scannerItems->reward)->setState(inventory::Item::State::Default);
    responseQueue.add(response::ItemUnhidden{ scannerItems->reward });
    responseQueue.add(response::XRayItemClaimed{ scannerItems->reward });
}

ItemIterator RequestHandler::operator()(const request::RemoveItem& request) const
{
    const auto removedFromStorageUnit = storageUnitManager.onItemRemoval(request.item, [this, it = request.item](const auto& storedItem) {
        if (storedItem != it)
            removeItemInternal(storedItem);
    });

    if (removedFromStorageUnit.has_value()) {
        storageUnitHandler.removeFromStorageUnit(request.item, *removedFromStorageUnit);
    }

    return removeItemInternal(request.item);
}

ItemIterator RequestHandler::removeItemInternal(ItemIterator it) const
{
    const auto itemID = itemIDMap.remove(it);
    loadout.unequipItem(it);
    responseQueue.removeResponsesReferencingItem(it);
    xRayScanner.onItemRemoval(it);
    const auto newIterator = inventory.erase(it);
    if (itemID.has_value())
        responseQueue.add(response::ItemRemoved{ *itemID });
    return newIterator;
}

}
