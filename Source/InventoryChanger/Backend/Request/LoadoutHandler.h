#pragma once

#include <InventoryChanger/Backend/Item.h>
#include <InventoryChanger/Backend/Loadout.h>
#include <InventoryChanger/Backend/Response/ResponseQueue.h>

#include <CSGO/Entity.h>

namespace inventory_changer::backend
{

template <typename ResponseAccumulator>
class LoadoutHandler {
public:
    LoadoutHandler(Loadout& loadout, ResponseAccumulator responseAccumulator)
        : loadout{ loadout }, responseAccumulator{ responseAccumulator } {}

    void equipItem(ItemIterator item, Loadout::Slot slot, csgo::Team team) const
    {
        if (markItemEquipped(item, slot, team))
            responseAccumulator(response::ItemEquipped{ item, slot, team });
    }

    bool markItemEquipped(ItemIterator item, Loadout::Slot slot, csgo::Team team) const
    {
        switch (team) {
        case csgo::Team::None:
            loadout.equipItemNoTeam(item, slot);
            return true;
        case csgo::Team::TT:
            loadout.equipItemTT(item, slot);
            return true;
        case csgo::Team::CT:
            loadout.equipItemCT(item, slot);
            return true;
        default:
            return false;
        }
    }

private:
    Loadout& loadout;
    ResponseAccumulator responseAccumulator;
};

}
