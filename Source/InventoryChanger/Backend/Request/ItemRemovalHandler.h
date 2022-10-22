#pragma once

#include <InventoryChanger/Backend/Item.h>
#include <InventoryChanger/Backend/Items.h>
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
    explicit ItemRemovalHandler(Items& items) : items{ items } {}

    ItemIterator operator()(ItemIterator item) const
    {
        const auto removedFromStorageUnit = items.storageUnitManager.onItemRemoval(item, [this, item](const auto& storedItem) {
            if (storedItem != item)
                removeItem(storedItem);
        });

        if (removedFromStorageUnit.has_value()) {
            StorageUnitHandler<ResponseAccumulator> storageUnitHandler{ items.storageUnitManager, ItemConstRemover{ items.inventory }, InventoryHandler{ items.inventory, ResponseAccumulator{ items.responseQueue } }, ResponseAccumulator{ items.responseQueue } };

            storageUnitHandler.removeFromStorageUnit(item, *removedFromStorageUnit);
        }

        return removeItem(item);
    }

private:
    ItemIterator removeItem(ItemIterator it) const
    {
        const auto itemID = items.itemIDMap.remove(it);
        items.loadout.unequipItem(it);
        items.responseQueue.removeResponsesReferencingItem(it);
        items.xRayScanner.onItemRemoval(it);
        const auto newIterator = items.inventory.erase(it);
        if (itemID.has_value())
            items.responseQueue.add(response::ItemRemoved{ *itemID });
        return newIterator;
    }

    Items& items;
};

}
