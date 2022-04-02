#pragma once

#include <InventoryChanger/Inventory/Storage.h>

#include "Loadout.h"

namespace inventory_changer::backend
{

class BackendSimulator {
public:
    [[nodiscard]] const Loadout& getLoadout() const noexcept
    {
        return loadout;
    }

    [[nodiscard]] const inventory::Storage& getInventory() const noexcept
    {
        return inventory;
    }

    void equipItemCT(Loadout::InventoryItemIndex index, Loadout::Slot slot)
    {
        loadout.equipItemCT(index, slot);
    }

    void equipItemTT(Loadout::InventoryItemIndex index, Loadout::Slot slot)
    {
        loadout.equipItemTT(index, slot);
    }

    void equipItemNoTeam(Loadout::InventoryItemIndex index, Loadout::Slot slot)
    {
        loadout.equipItemNoTeam(index, slot);
    }

    [[nodiscard]] static BackendSimulator& instance()
    {
        static BackendSimulator backendSimulator;
        return backendSimulator;
    }

    void clearInventory()
    {
        inventory.clear();
    }

    void addItem(inventory::Item_v2 item)
    {
        inventory.addItem(std::move(item));
    }

    std::vector<inventory::Item_v2>::iterator removeItem(std::vector<inventory::Item_v2>::const_iterator it)
    {
        return inventory.removeItem(it);
    }

private:
    inventory::Storage inventory;
    Loadout loadout;
};

}
