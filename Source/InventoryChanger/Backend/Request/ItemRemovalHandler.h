#pragma once

#include <InventoryChanger/Backend/Item.h>
#include <InventoryChanger/Backend/ItemIDMap.h>
#include <InventoryChanger/Backend/Loadout.h>
#include <InventoryChanger/Backend/Request/StorageUnitHandler.h>
#include <InventoryChanger/Backend/Response/ResponseQueue.h>
#include <InventoryChanger/Backend/StorageUnitManager.h>
#include <InventoryChanger/Backend/XRayScanner.h>

namespace inventory_changer::backend
{

template <typename ResponseAccumulator>
class ItemRemovalHandler {
public:
    ItemRemovalHandler(StorageUnitManager& storageUnitManager, XRayScanner& xRayScanner, ResponseQueue<>& responseQueue, ItemList& inventory, Loadout& loadout, ItemIDMap& itemIDMap, StorageUnitHandler<ResponseAccumulator> storageUnitHandler)
        : storageUnitManager{ storageUnitManager }, xRayScanner{ xRayScanner }, responseQueue{ responseQueue }, inventory{ inventory }, loadout{ loadout }, itemIDMap{ itemIDMap }, storageUnitHandler{ storageUnitHandler } {}

    ItemIterator removeItem(ItemIterator item) const
    {
        const auto removedFromStorageUnit = storageUnitManager.onItemRemoval(item, [this, item](const auto& storedItem) {
            if (storedItem != item)
                removeItemInternal(storedItem);
        });

        if (removedFromStorageUnit.has_value()) {
            storageUnitHandler.removeFromStorageUnit(item, *removedFromStorageUnit);
        }

        return removeItemInternal(item);
    }

private:
    ItemIterator removeItemInternal(ItemIterator it) const
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

    StorageUnitManager& storageUnitManager;
    XRayScanner& xRayScanner;
    ResponseQueue<>& responseQueue;
    ItemList& inventory;
    Loadout& loadout;
    ItemIDMap& itemIDMap;
    StorageUnitHandler<ResponseAccumulator> storageUnitHandler;
};

}
