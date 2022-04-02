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
        responses.emplace(Response::Type::ItemAdded, std::prev(inventory.end()));
    }

    std::list<inventory::Item_v2>::const_iterator removeItem(std::list<inventory::Item_v2>::const_iterator it)
    {
        const auto newIterator = inventory.removeItem(it);
        responses.emplace(Response::Type::ItemRemoved, it);
        return newIterator;
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
    inventory::Storage inventory;
    Loadout loadout;
    std::queue<Response> responses;
};

}
