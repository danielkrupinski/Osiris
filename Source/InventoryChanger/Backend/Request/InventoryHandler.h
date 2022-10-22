#pragma once

#include <InventoryChanger/Backend/Item.h>
#include <InventoryChanger/Backend/Response/ResponseTypes.h>
#include <InventoryChanger/Backend/Response/ResponseAccumulator.h>

namespace inventory_changer::backend
{

class InventoryHandler {
public:
    InventoryHandler(ItemList& inventory, ResponseAccumulator responseAccumulator)
        : inventory{ inventory }, responseAccumulator{ responseAccumulator } {}

    void moveItemToFront(ItemIterator item) const
    {
        inventory.splice(inventory.end(), inventory, item);
        responseAccumulator(response::ItemMovedToFront{ item });
    }

    ItemIterator addItem(inventory::Item item, bool asUnacknowledged) const
    {
        inventory.push_back(std::move(item));
        const auto added = std::prev(inventory.end());
        responseAccumulator(response::ItemAdded{ added, asUnacknowledged });
        return added;
    }

private:
    ItemList& inventory;
    ResponseAccumulator responseAccumulator;
};

}
