#pragma once

#include "InventoryHandler.h"
#include "RequestTypes.h"
#include "StorageUnitHandler.h"
#include <InventoryChanger/Backend/Item.h>
#include <InventoryChanger/Backend/Response/Response.h>
#include <InventoryChanger/Backend/Response/ResponseAccumulator.h>
#include <InventoryChanger/Backend/Response/ResponseQueue.h>
#include <InventoryChanger/GameItems/CrateLootLookup.h>
#include <InventoryChanger/GameItems/Lookup.h>

namespace inventory_changer::backend
{

class ItemIDMap;
class Loadout;
class PickEm;
class StorageUnitManager;
class XRayScanner;

struct RequestHandler {
    RequestHandler(InventoryHandler<ResponseAccumulator> inventoryHandler, StorageUnitHandler<ResponseAccumulator> storageUnitHandler, StorageUnitManager& storageUnitManager, XRayScanner& xRayScanner, ResponseQueue<>& responseQueue, ItemList& inventory, Loadout& loadout, ItemIDMap& itemIDMap, const game_items::Lookup& gameItemLookup, const game_items::CrateLootLookup& crateLootLookup, ItemConstRemover constRemover)
        : inventoryHandler{ inventoryHandler }, storageUnitHandler{ storageUnitHandler }, storageUnitManager{ storageUnitManager }, xRayScanner{ xRayScanner }, responseQueue{ responseQueue }, inventory{ inventory }, loadout{ loadout }, itemIDMap{ itemIDMap }, gameItemLookup{ gameItemLookup }, crateLootLookup{ crateLootLookup }, constRemover{ constRemover } {}

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
    void operator()(const request::PerformXRayScan& request) const;
    void operator()(const request::ClaimXRayScannedItem& request) const;
    ItemIterator operator()(const request::RemoveItem& request) const;

private:
    ItemIterator removeItemInternal(ItemIterator it) const;

    InventoryHandler<ResponseAccumulator> inventoryHandler;
    StorageUnitHandler<ResponseAccumulator> storageUnitHandler;
    StorageUnitManager& storageUnitManager;
    XRayScanner& xRayScanner;
    ResponseQueue<>& responseQueue;
    ItemList& inventory;
    Loadout& loadout;
    ItemIDMap& itemIDMap;
    const game_items::Lookup& gameItemLookup;
    const game_items::CrateLootLookup& crateLootLookup;
    ItemConstRemover constRemover;
};

}
