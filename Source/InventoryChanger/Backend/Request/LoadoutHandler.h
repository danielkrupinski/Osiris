#pragma once

#include <InventoryChanger/Backend/Item.h>
#include <InventoryChanger/Backend/Loadout.h>
#include <InventoryChanger/Backend/Response/ResponseQueue.h>

#include <SDK/Entity.h>

namespace inventory_changer::backend
{

class LoadoutHandler {
public:
    LoadoutHandler(Loadout& loadout, ResponseQueue<>& responseQueue)
        : loadout{ loadout }, responseQueue{ responseQueue } {}

    void equipItem(ItemIterator item, Loadout::Slot slot, Team team) const
    {
        switch (team) {
        case Team::None:
            loadout.equipItemNoTeam(item, slot);
            break;
        case Team::TT:
            loadout.equipItemTT(item, slot);
            break;
        case Team::CT:
            loadout.equipItemCT(item, slot);
            break;
        default:
            return;
        }

        responseQueue.add(response::ItemEquipped{ item, slot, team });
    }

    void markItemEquipped(ItemIterator item, Loadout::Slot slot, Team team) const
    {
        switch (team) {
        case Team::None:
            loadout.equipItemNoTeam(item, slot);
            break;
        case Team::TT:
            loadout.equipItemTT(item, slot);
            break;
        case Team::CT:
            loadout.equipItemCT(item, slot);
            break;
        default:
            break;
        }
    }

private:
    Loadout& loadout;
    ResponseQueue<>& responseQueue;
};

}
