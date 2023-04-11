#pragma once

#include <cassert>
#include <ctime>

#include <InventoryChanger/Backend/Item.h>
#include <InventoryChanger/Backend/StorageUnitManager.h>

#include "InventoryHandler.h"

namespace inventory_changer::backend
{

template <typename ResponseAccumulator>
class StorageUnitHandler {
public:
    StorageUnitHandler(StorageUnitManager& storageUnitManager, ItemConstRemover constRemover, InventoryHandler inventoryHandler, ResponseAccumulator responseAccumulator)
        : storageUnitManager{ storageUnitManager }, constRemover{ constRemover }, inventoryHandler{ inventoryHandler }, responseAccumulator{ responseAccumulator } {}

    void nameStorageUnit(ItemIterator storageUnitIterator, std::string_view name) const
    {
        assert(storageUnitIterator->gameItem().isStorageUnit());

        const auto storageUnit = constRemover(storageUnitIterator).getOrCreate<inventory::StorageUnit>();
        if (!storageUnit)
            return;

        storageUnit->name = name;
        markStorageUnitModified(storageUnitIterator);
        inventoryHandler.moveItemToFront(storageUnitIterator);

        responseAccumulator(response::StorageUnitNamed{ storageUnitIterator });
    }

    void markStorageUnitModified(ItemIterator storageUnitIterator) const
    {
        assert(storageUnitIterator->gameItem().isStorageUnit());

        const auto storageUnit = constRemover(storageUnitIterator).getOrCreate<inventory::StorageUnit>();
        if (!storageUnit)
            return;

        storageUnit->modificationDateTimestamp = static_cast<std::uint32_t>(std::time(nullptr));

        responseAccumulator(response::StorageUnitModified{ storageUnitIterator });
    }

    void bindItemToStorageUnit(ItemIterator item, ItemIterator storageUnitIterator) const
    {
        assert(!item->gameItem().isStorageUnit() && storageUnitIterator->gameItem().isStorageUnit());

        const auto storageUnit = constRemover(storageUnitIterator).getOrCreate<inventory::StorageUnit>();
        if (!storageUnit)
            return;

        ++storageUnit->itemCount;
        storageUnitManager.addItemToStorageUnit(item, storageUnitIterator);
        constRemover(item).setState(inventory::Item::State::InStorageUnit);
        updateStorageUnitAttributes(storageUnitIterator);

        responseAccumulator(response::ItemBoundToStorageUnit{ item, storageUnitIterator });
    }

    void addItemToStorageUnit(ItemIterator item, ItemIterator storageUnitIterator) const
    {
        assert(!item->gameItem().isStorageUnit() && storageUnitIterator->gameItem().isStorageUnit());

        bindItemToStorageUnit(item, storageUnitIterator);
        markStorageUnitModified(storageUnitIterator);
        responseAccumulator(response::ItemAddedToStorageUnit{ storageUnitIterator });
    }

    void removeFromStorageUnit(ItemIterator item, ItemIterator storageUnitIterator) const
    {
        assert(!item->gameItem().isStorageUnit() && storageUnitIterator->gameItem().isStorageUnit());

        const auto storageUnit = constRemover(storageUnitIterator).getOrCreate<inventory::StorageUnit>();
        if (!storageUnit)
            return;

        --storageUnit->itemCount;
        storageUnitManager.removeItemFromStorageUnit(item, storageUnitIterator);
        constRemover(item).setState(inventory::Item::State::Default);
        markStorageUnitModified(storageUnitIterator);

        responseAccumulator(response::ItemRemovedFromStorageUnit{ item, storageUnitIterator });
    }

    void updateStorageUnitAttributes(ItemIterator storageUnit) const
    {
        assert(storageUnit->gameItem().isStorageUnit());
        responseAccumulator(response::StorageUnitModified{ storageUnit });
    }

    void loadStorageUnitContents(ItemIterator storageUnit) const
    {
        assert(storageUnit->gameItem().isStorageUnit());
        responseAccumulator(response::StorageUnitContentsLoaded{ storageUnit });
    }

private:
    StorageUnitManager& storageUnitManager;
    ItemConstRemover constRemover;
    InventoryHandler inventoryHandler;
    ResponseAccumulator responseAccumulator;
};

}
