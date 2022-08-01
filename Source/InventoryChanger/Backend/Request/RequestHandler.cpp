#include "RequestHandler.h"

#include <InventoryChanger/Backend/BackendSimulator.h>
#include <InventoryChanger/GameItems/Lookup.h>
#include <InventoryChanger/ItemGenerator/ItemGenerator.h>

namespace inventory_changer::backend
{

Response RequestHandler::operator()(const request::ApplySticker& request) const
{
    const auto skin = constRemover.removeConstness(request.item)->get<inventory::Skin>();
    if (!skin)
        return {};

    skin->stickers[request.slot].stickerID = backend.getGameItemLookup().getStorage().getStickerKit(request.sticker->gameItem()).id;
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

    const auto statTrakFrom = inventory::getStatTrak(*request.itemFrom);
    if (!statTrakFrom)
        return {};

    const auto statTrakTo = inventory::getStatTrak(*request.itemTo);
    if (!statTrakTo)
        return {};

    backend.removeItem(request.statTrakSwapTool);
    backend.getRequestor().request<request::UpdateStatTrak>(request.itemFrom, *statTrakTo);
    backend.getRequestor().request<request::UpdateStatTrak>(request.itemTo, *statTrakFrom);
    backend.moveToFront(request.itemFrom);
    backend.moveToFront(request.itemTo);
    backend.getRequestor().request<request::MarkItemUpdated>(*statTrakFrom >= *statTrakTo ? request.itemFrom : request.itemTo);
    return response::StatTrakSwapped{ *statTrakFrom < *statTrakTo ? request.itemFrom : request.itemTo };
}

Response RequestHandler::operator()(const request::OpenContainer& request) const
{
    if (!request.container->gameItem().isCase())
        return {};

    auto generatedItem = item_generator::generateItemFromContainer(backend.getGameItemLookup(), backend.getCrateLootLookup(), *request.container);
    if (!generatedItem.has_value())
        return {};

    if (request.key.has_value()) {
        if (const auto& keyItem = *request.key; keyItem->gameItem().isCaseKey())
            backend.removeItem(keyItem);
    }

    backend.removeItem(request.container);
    const auto receivedItem = backend.addItemUnacknowledged(std::move(*generatedItem));
    return response::ContainerOpened{ receivedItem };
}

Response RequestHandler::operator()(const request::ApplyPatch& request) const
{
    const auto agent = constRemover.removeConstness(request.item)->getOrCreate<inventory::Agent>();
    if (!agent)
        return {};

    agent->patches[request.slot].patchID = backend.getGameItemLookup().getStorage().getPatch(request.patch->gameItem()).id;
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
    if (const auto operationCoin = backend.getGameItemLookup().findItem(coinID); operationCoin.has_value()) {
        backend.addItemUnacknowledged(inventory::Item{ *operationCoin });
        backend.removeItem(request.operationPass);
    }
    return {};
}

Response RequestHandler::operator()(const request::ActivateViewerPass& request) const
{
    const auto& gameItem = request.item->gameItem();
    if (!gameItem.isViewerPass())
        return {};

    const auto coinID = Helpers::bronzeEventCoinFromViewerPass(gameItem.getWeaponID());
    if (coinID == WeaponId::None)
        return {};

    if (const auto eventCoin = backend.getGameItemLookup().findItem(coinID); eventCoin.has_value()) {
        const auto addedEventCoin = backend.addItemUnacknowledged(inventory::Item{ *eventCoin, inventory::TournamentCoin{ Helpers::numberOfTokensWithViewerPass(gameItem.getWeaponID()) } });
        backend.removeItem(request.item);
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
    if (!request.item->gameItem().isSouvenirToken())
        return {};

    const auto tournamentEventID = backend.getGameItemLookup().getStorage().getTournamentEventID(request.item->gameItem());
    const auto& inventory = backend.getInventory();
    const auto tournamentCoin = std::ranges::find_if(inventory, [this, tournamentEventID](const auto& item) { return item.gameItem().isTournamentCoin() && backend.getGameItemLookup().getStorage().getTournamentEventID(item.gameItem()) == tournamentEventID; });
    if (tournamentCoin == inventory.end())
        return {};

    const auto tournamentCoinData = constRemover.removeConstness(tournamentCoin)->getOrCreate<inventory::TournamentCoin>();
    if (!tournamentCoinData)
        return {};

    ++tournamentCoinData->dropsAwarded;
    backend.removeItem(request.item);
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
    return response::StatTrakUpdated{ request.item, request.newStatTrak };
}

Response RequestHandler::operator()(const request::SelectTeamGraffiti& request) const
{
    if (request.tournamentCoin->gameItem().isTournamentCoin())
        return response::TeamGraffitiSelected{ request.tournamentCoin, request.graffitiID };
    return {};
}

Response RequestHandler::operator()(const request::MarkItemUpdated& request) const
{
    return response::ItemUpdated{ request.item };
}

Response RequestHandler::operator()(const request::PickStickerPickEm& request) const
{
    pickEm.pick(request.position, request.team);
    return response::PickEmUpdated{};
}

Response RequestHandler::operator()(const request::HideItem& request) const
{
    constRemover.removeConstness(request.item)->setState(inventory::Item::State::InXrayScanner);
    return response::ItemHidden{ request.item };
}

Response RequestHandler::operator()(const request::UnhideItem& request) const
{
    constRemover.removeConstness(request.item)->setState(inventory::Item::State::Default);
    return response::ItemUnhidden{ request.item };
}

Response RequestHandler::operator()(const request::PerformXRayScan& request) const
{
    if (!request.crate->gameItem().isCase())
        return {};

    auto generatedItem = item_generator::generateItemFromContainer(backend.getGameItemLookup(), backend.getCrateLootLookup(), *request.crate);
    if (!generatedItem.has_value())
        return {};

    constRemover.removeConstness(request.crate)->setState(inventory::Item::State::InXrayScanner);
    backend.getRequestor().request<request::HideItem>(request.crate);

    generatedItem->setState(inventory::Item::State::InXrayScanner);

    const auto receivedItem = backend.addItemUnacknowledged(std::move(*generatedItem));
    xRayScanner.storeItems(XRayScanner::Items{ receivedItem, request.crate });
    return response::XRayScannerUsed{ receivedItem };
}

Response RequestHandler::operator()(const request::ClaimXRayScannedItem& request) const
{
    const auto scannerItems = xRayScanner.getItems();
    if (!scannerItems.has_value())
        return {};

    if (request.container != scannerItems->crate)
        return {};

    if (request.key.has_value()) {
        if (const auto& keyItem = *request.key; keyItem->gameItem().isCaseKey())
            backend.removeItem(keyItem);
    }

    backend.removeItem(request.container);
    backend.getRequestor().request<request::UnhideItem>(scannerItems->reward);
    return response::XRayItemClaimed{ scannerItems->reward };
}

Response RequestHandler::operator()(const request::NameStorageUnit& request) const
{
    const auto storageUnit = constRemover.removeConstness(request.storageUnit)->getOrCreate<inventory::StorageUnit>();
    if (!storageUnit)
        return {};

    storageUnit->name = request.name;
    backend.getRequestor().request<request::MarkStorageUnitModified>(request.storageUnit);
    backend.moveToFront(request.storageUnit);

    return response::StorageUnitNamed{ request.storageUnit };
}

Response RequestHandler::operator()(const request::MarkStorageUnitModified& request) const
{
    const auto storageUnit = constRemover.removeConstness(request.storageUnit)->getOrCreate<inventory::StorageUnit>();
    if (!storageUnit)
        return {};

    storageUnit->modificationDateTimestamp = static_cast<std::uint32_t>(std::time(nullptr));

    return response::StorageUnitModified{ request.storageUnit };
}

Response RequestHandler::operator()(const request::BindItemToStorageUnit& request) const
{
    if (!request.storageUnit->gameItem().isStorageUnit())
        return {};

    const auto storageUnit = constRemover.removeConstness(request.storageUnit)->getOrCreate<inventory::StorageUnit>();
    if (!storageUnit)
        return {};

    ++storageUnit->itemCount;
    storageUnitManager.addItemToStorageUnit(request.item, request.storageUnit);
    constRemover.removeConstness(request.item)->setState(inventory::Item::State::InStorageUnit);
    backend.getRequestor().request<request::UpdateStorageUnitAttributes>(request.storageUnit);

    return response::ItemBoundToStorageUnit{ request.item, request.storageUnit };
}

Response RequestHandler::operator()(const request::AddItemToStorageUnit& request) const
{
    backend.getRequestor().request<request::BindItemToStorageUnit>(request.item, request.storageUnit);
    backend.getRequestor().request<request::MarkStorageUnitModified>(request.storageUnit);
    return response::ItemAddedToStorageUnit{ request.storageUnit };
}

Response RequestHandler::operator()(const request::RemoveFromStorageUnit& request) const
{
    if (!request.storageUnit->gameItem().isStorageUnit())
        return {};

    const auto storageUnit = constRemover.removeConstness(request.storageUnit)->getOrCreate<inventory::StorageUnit>();
    if (!storageUnit)
        return {};

    --storageUnit->itemCount;
    storageUnitManager.removeItemFromStorageUnit(request.item, request.storageUnit);
    constRemover.removeConstness(request.item)->setState(inventory::Item::State::Default);
    backend.getRequestor().request<request::MarkStorageUnitModified>(request.storageUnit);

    return response::ItemRemovedFromStorageUnit{ request.item, request.storageUnit };
}

Response RequestHandler::operator()(const request::UpdateStorageUnitAttributes& request) const
{
    if (!request.storageUnit->gameItem().isStorageUnit())
        return {};

    return response::StorageUnitModified{ request.storageUnit };
}

}
