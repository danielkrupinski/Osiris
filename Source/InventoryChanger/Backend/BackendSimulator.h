#pragma once

#include <queue>
#include <variant>

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

    [[nodiscard]] const std::list<inventory::Item_v2>& getInventory() const noexcept
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
        inventory.push_back(std::move(item));
        responses.emplace(Response::Type::ItemAdded, std::prev(inventory.end()));
    }

    std::list<inventory::Item_v2>::const_iterator removeItem(std::list<inventory::Item_v2>::const_iterator it)
    {
        const auto newIterator = inventory.erase(it);
        responses.emplace(Response::Type::ItemRemoved, it);
        return newIterator;
    }

    void updateStatTrak(std::list<inventory::Item_v2>::const_iterator it, int newStatTrak)
    {
        updateStatTrak(removeConstness(it), newStatTrak);
    }

    struct Response {
        enum class Type {
            ItemAdded,
            ItemRemoved
        };

        Type type;
        std::variant<std::list<inventory::Item_v2>::const_iterator> data;
    };

    template <typename ResponseHandler>
    void run(ResponseHandler responseHandler)
    {
        while (!responses.empty()) {
            responseHandler(responses.front());
            responses.pop();
        }
    }

private:
    static bool updateStatTrak(std::list<inventory::Item_v2>::iterator item, int newStatTrak)
    {
        if (const auto skin = item->get<inventory::Skin>()) {
            skin->statTrak = newStatTrak;
            return true;
        }

        if (const auto music = item->get<inventory::Music>()) {
            music->statTrak = newStatTrak;
            return true;
        }

        return false;
    }

    [[nodiscard]] std::list<inventory::Item_v2>::iterator removeConstness(std::list<inventory::Item_v2>::const_iterator it)
    {
        return inventory.erase(it, it);
    }

    std::list<inventory::Item_v2> inventory;
    Loadout loadout;
    std::queue<Response> responses;
};

}
