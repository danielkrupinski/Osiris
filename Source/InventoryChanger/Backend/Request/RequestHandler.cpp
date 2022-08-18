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

    skin->stickers[request.slot].stickerID = backend.getGameItemLookup().getStorage().getStickerKit(request.sticker->gameItem()).id;
    skin->stickers[request.slot].wear = 0.0f;

    backend.moveToFront(request.item);
    backend.removeItem(request.sticker);
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

    backend.removeItem(request.statTrakSwapTool);
    backend.getRequestor().request<request::UpdateStatTrak>(request.itemFrom, *statTrakTo);
    backend.getRequestor().request<request::UpdateStatTrak>(request.itemTo, *statTrakFrom);
    backend.moveToFront(request.itemFrom);
    backend.moveToFront(request.itemTo);
    backend.getRequestor().request<request::MarkItemUpdated>(*statTrakFrom >= *statTrakTo ? request.itemFrom : request.itemTo);
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

    auto generatedItem = item_generator::generateItemFromContainer(backend.getGameItemLookup(), backend.getCrateLootLookup(), *request.container, toPointer(request.key));
    if (!generatedItem.has_value())
        return;

    if (request.key.has_value()) {
        if (const auto& keyItem = *request.key; keyItem->gameItem().isCaseKey())
            backend.removeItem(keyItem);
    }

    backend.removeItem(request.container);
    const auto receivedItem = backend.addItemUnacknowledged(std::move(*generatedItem));
    responseQueue.add(response::ContainerOpened{ receivedItem });
}

void RequestHandler::operator()(const request::ApplyPatch& request) const
{
    const auto agent = constRemover.removeConstness(request.item)->getOrCreate<inventory::Agent>();
    if (!agent)
        return;

    agent->patches[request.slot].patchID = backend.getGameItemLookup().getStorage().getPatch(request.patch->gameItem()).id;
    backend.moveToFront(request.item);
    backend.removeItem(request.patch);
    responseQueue.add(response::PatchApplied{ request.item, request.slot });
}

void RequestHandler::operator()(const request::RemovePatch& request) const
{
    const auto agent = get<inventory::Agent>(*constRemover.removeConstness(request.item));
    if (!agent)
        return;

    agent->patches[request.slot].patchID = 0;
    backend.moveToFront(request.item);
    responseQueue.add(response::PatchRemoved{ request.item, request.slot });
}

void RequestHandler::operator()(const request::ActivateOperationPass& request) const
{
    const auto& gameItem = request.operationPass->gameItem();
    if (!gameItem.isOperationPass())
        return;

    const auto coinID = gameItem.getWeaponID() != WeaponId::OperationHydraPass ? static_cast<WeaponId>(static_cast<int>(gameItem.getWeaponID()) + 1) : WeaponId::BronzeOperationHydraCoin;
    if (const auto operationCoin = backend.getGameItemLookup().findItem(coinID)) {
        backend.addItemUnacknowledged(inventory::Item{ *operationCoin });
        backend.removeItem(request.operationPass);
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

    if (const auto eventCoin = backend.getGameItemLookup().findItem(coinID)) {
        const auto addedEventCoin = backend.addItemUnacknowledged(inventory::Item{ *eventCoin, inventory::TournamentCoin{ Helpers::numberOfTokensWithViewerPass(gameItem.getWeaponID()) } });
        backend.removeItem(request.item);
        responseQueue.add(response::ViewerPassActivated{ addedEventCoin });
    }
}

void RequestHandler::operator()(const request::AddNameTag& request) const
{
    const auto skin = get<inventory::Skin>(*constRemover.removeConstness(request.item));
    if (!skin)
        return;

    skin->nameTag = request.nameTag;
    backend.removeItem(request.nameTagItem);
    backend.moveToFront(request.item);
    responseQueue.add(response::NameTagAdded{ request.item });
}

void RequestHandler::operator()(const request::RemoveNameTag& request) const
{
    if (const auto skin = get<inventory::Skin>(*constRemover.removeConstness(request.item))) {
        skin->nameTag.clear();
        backend.moveToFront(request.item);
        responseQueue.add(response::NameTagRemoved{ request.item });
    }
}

void RequestHandler::operator()(const request::ActivateSouvenirToken& request) const
{
    if (!request.item->gameItem().isSouvenirToken())
        return;

    const auto tournamentEventID = backend.getGameItemLookup().getStorage().getTournamentEventID(request.item->gameItem());
    const auto& inventory = backend.getInventory();
    const auto tournamentCoin = std::ranges::find_if(inventory, [this, tournamentEventID](const auto& item) { return item.gameItem().isTournamentCoin() && backend.getGameItemLookup().getStorage().getTournamentEventID(item.gameItem()) == tournamentEventID; });
    if (tournamentCoin == inventory.end())
        return;

    const auto tournamentCoinData = constRemover.removeConstness(tournamentCoin)->getOrCreate<inventory::TournamentCoin>();
    if (!tournamentCoinData)
        return;

    ++tournamentCoinData->dropsAwarded;
    backend.removeItem(request.item);
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

    backend.moveToFront(request.item);
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

void RequestHandler::operator()(const request::MarkItemUpdated& request) const
{
    responseQueue.add(response::ItemUpdated{ request.item });
}

void RequestHandler::operator()(const request::PickStickerPickEm& request) const
{
    pickEm.pick(request.position, request.team);
    responseQueue.add(response::PickEmUpdated{});
}

void RequestHandler::operator()(const request::HideItem& request) const
{
    constRemover.removeConstness(request.item)->setState(inventory::Item::State::InXrayScanner);
    responseQueue.add(response::ItemHidden{ request.item });
}

void RequestHandler::operator()(const request::UnhideItem& request) const
{
    constRemover.removeConstness(request.item)->setState(inventory::Item::State::Default);
    responseQueue.add(response::ItemUnhidden{ request.item });
}

void RequestHandler::operator()(const request::PerformXRayScan& request) const
{
    if (!request.crate->gameItem().isCrate())
        return;

    auto generatedItem = item_generator::generateItemFromContainer(backend.getGameItemLookup(), backend.getCrateLootLookup(), *request.crate, nullptr);
    if (!generatedItem.has_value())
        return;

    constRemover.removeConstness(request.crate)->setState(inventory::Item::State::InXrayScanner);
    backend.getRequestor().request<request::HideItem>(request.crate);

    generatedItem->setState(inventory::Item::State::InXrayScanner);

    const auto receivedItem = backend.addItemUnacknowledged(std::move(*generatedItem));
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
            backend.removeItem(keyItem);
        }
    }

    backend.removeItem(request.container);
    backend.getRequestor().request<request::UnhideItem>(scannerItems->reward);
    responseQueue.add(response::XRayItemClaimed{ scannerItems->reward });
}

void RequestHandler::operator()(const request::NameStorageUnit& request) const
{
    const auto storageUnit = constRemover.removeConstness(request.storageUnit)->getOrCreate<inventory::StorageUnit>();
    if (!storageUnit)
        return;

    storageUnit->name = request.name;
    backend.getRequestor().request<request::MarkStorageUnitModified>(request.storageUnit);
    backend.moveToFront(request.storageUnit);

    responseQueue.add(response::StorageUnitNamed{ request.storageUnit });
}

void RequestHandler::operator()(const request::MarkStorageUnitModified& request) const
{
    const auto storageUnit = constRemover.removeConstness(request.storageUnit)->getOrCreate<inventory::StorageUnit>();
    if (!storageUnit)
        return;

    storageUnit->modificationDateTimestamp = static_cast<std::uint32_t>(std::time(nullptr));

    responseQueue.add(response::StorageUnitModified{ request.storageUnit });
}

void RequestHandler::operator()(const request::BindItemToStorageUnit& request) const
{
    if (!request.storageUnit->gameItem().isStorageUnit())
        return;

    const auto storageUnit = constRemover.removeConstness(request.storageUnit)->getOrCreate<inventory::StorageUnit>();
    if (!storageUnit)
        return;

    ++storageUnit->itemCount;
    storageUnitManager.addItemToStorageUnit(request.item, request.storageUnit);
    constRemover.removeConstness(request.item)->setState(inventory::Item::State::InStorageUnit);
    backend.getRequestor().request<request::UpdateStorageUnitAttributes>(request.storageUnit);

    responseQueue.add(response::ItemBoundToStorageUnit{ request.item, request.storageUnit });
}

void RequestHandler::operator()(const request::AddItemToStorageUnit& request) const
{
    backend.getRequestor().request<request::BindItemToStorageUnit>(request.item, request.storageUnit);
    backend.getRequestor().request<request::MarkStorageUnitModified>(request.storageUnit);
    responseQueue.add(response::ItemAddedToStorageUnit{ request.storageUnit });
}

void RequestHandler::operator()(const request::RemoveFromStorageUnit& request) const
{
    if (!request.storageUnit->gameItem().isStorageUnit())
        return;

    const auto storageUnit = constRemover.removeConstness(request.storageUnit)->getOrCreate<inventory::StorageUnit>();
    if (!storageUnit)
        return;

    --storageUnit->itemCount;
    storageUnitManager.removeItemFromStorageUnit(request.item, request.storageUnit);
    constRemover.removeConstness(request.item)->setState(inventory::Item::State::Default);
    backend.getRequestor().request<request::MarkStorageUnitModified>(request.storageUnit);

    responseQueue.add(response::ItemRemovedFromStorageUnit{ request.item, request.storageUnit });
}

void RequestHandler::operator()(const request::UpdateStorageUnitAttributes& request) const
{
    if (!request.storageUnit->gameItem().isStorageUnit())
        return;

    responseQueue.add(response::StorageUnitModified{ request.storageUnit });
}

}
