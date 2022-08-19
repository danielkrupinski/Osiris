#pragma once

#include "RequestTypes.h"
#include <InventoryChanger/Backend/Item.h>
#include <InventoryChanger/Backend/Response/Response.h>
#include <InventoryChanger/Backend/Response/ResponseQueue.h>

namespace inventory_changer::backend
{

class BackendSimulator;
class ItemIDMap;
class Loadout;
class PickEm;
class StorageUnitManager;
class XRayScanner;

struct RequestHandler {
    RequestHandler(BackendSimulator& backend, PickEm& pickEm, StorageUnitManager& storageUnitManager, XRayScanner& xRayScanner, ResponseQueue<>& responseQueue, ItemList& inventory, Loadout& loadout, ItemIDMap& itemIDMap, ItemConstRemover constRemover)
        : backend{ backend }, pickEm{ pickEm }, storageUnitManager{ storageUnitManager }, xRayScanner{ xRayScanner }, responseQueue{ responseQueue }, inventory{ inventory }, loadout{ loadout }, constRemover{ constRemover }, itemIDMap{ itemIDMap } {}

    void operator()(const request::ApplySticker& request) const;
    void operator()(const request::WearSticker& request) const;
    void operator()(const request::SwapStatTrak& request) const;
    void operator()(const request::OpenContainer& request) const;
    void operator()(const request::ApplyPatch& request) const;
    void operator()(const request::RemovePatch& request) const;
    void operator()(const request::ActivateOperationPass& request) const;
    void operator()(const request::ActivateViewerPass& request) const;
    void operator()(const request::AddNameTag& request) const;
    void operator()(const request::RemoveNameTag& request) const;
    void operator()(const request::ActivateSouvenirToken& request) const;
    void operator()(const request::UnsealGraffiti& request) const;
    void operator()(const request::UpdateStatTrak& request) const;
    void operator()(const request::SelectTeamGraffiti& request) const;
    void operator()(const request::MarkItemUpdated& request) const;
    void operator()(const request::PickStickerPickEm& request) const;
    void operator()(const request::HideItem& request) const;
    void operator()(const request::UnhideItem& request) const;
    void operator()(const request::PerformXRayScan& request) const;
    void operator()(const request::ClaimXRayScannedItem& request) const;
    void operator()(const request::NameStorageUnit& request) const;
    void operator()(const request::MarkStorageUnitModified& request) const;
    void operator()(const request::BindItemToStorageUnit& request) const;
    void operator()(const request::AddItemToStorageUnit& request) const;
    void operator()(const request::RemoveFromStorageUnit& request) const;
    void operator()(const request::UpdateStorageUnitAttributes& request) const;
    void operator()(const request::MoveItemToFront& request) const;
    ItemIterator operator()(request::AddItem request) const;
    ItemIterator operator()(const request::RemoveItem& request) const;

private:
    ItemIterator removeItemInternal(ItemIterator it) const;

    BackendSimulator& backend;
    PickEm& pickEm;
    StorageUnitManager& storageUnitManager;
    XRayScanner& xRayScanner;
    ResponseQueue<>& responseQueue;
    ItemList& inventory;
    Loadout& loadout;
    ItemIDMap& itemIDMap;
    ItemConstRemover constRemover;
};

}
