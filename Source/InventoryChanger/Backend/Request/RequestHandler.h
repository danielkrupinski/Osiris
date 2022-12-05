#pragma once

#include "InventoryHandler.h"
#include "ItemModificationHandler.h"
#include "ItemRemovalHandler.h"
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
    RequestHandler(ItemModificationHandler<ResponseAccumulator> itemModificationHandler, ItemRemovalHandler<ResponseAccumulator> itemRemovalHandler, InventoryHandler inventoryHandler, StorageUnitHandler<ResponseAccumulator> storageUnitHandler, ResponseAccumulator responseAccumulator, ItemList& inventory, const game_items::Lookup& gameItemLookup, ItemConstRemover constRemover)
        : itemModificationHandler{ itemModificationHandler }, itemRemovalHandler{ itemRemovalHandler }, inventoryHandler{ inventoryHandler }, storageUnitHandler{ storageUnitHandler }, responseAccumulator{ responseAccumulator }, inventory{ inventory }, gameItemLookup{ gameItemLookup }, constRemover{ constRemover } {}

    void operator()(const request::ApplySticker& request) const;
    void operator()(const request::WearSticker& request) const;
    void operator()(const request::SwapStatTrak& request) const;
    void operator()(const request::ApplyPatch& request) const;
    void operator()(const request::RemovePatch& request) const;
    void operator()(const request::AddNameTag& request) const;
    void operator()(const request::RemoveNameTag& request) const;
    void operator()(const request::ActivateSouvenirToken& request) const;
    void operator()(const request::UnsealGraffiti& request) const;

private:
    void copyTradability(ItemIterator source, ItemIterator destination) const;

    ItemModificationHandler<ResponseAccumulator> itemModificationHandler;
    ItemRemovalHandler<ResponseAccumulator> itemRemovalHandler;
    InventoryHandler inventoryHandler;
    StorageUnitHandler<ResponseAccumulator> storageUnitHandler;
    ResponseAccumulator responseAccumulator;
    ItemList& inventory;
    const game_items::Lookup& gameItemLookup;
    ItemConstRemover constRemover;
};

}
