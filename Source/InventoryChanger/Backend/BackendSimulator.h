#pragma once

#include <queue>
#include <variant>

#include "ItemIDMap.h"
#include "Loadout.h"
#include "Response.h"

namespace inventory_changer::backend
{

struct UseToolRequest {
    enum class Action {
        Use,
        WearSticker,
        RemoveNameTag
    };

    std::uint64_t toolItemID = 0;
    std::uint64_t destItemID = 0;
    std::uint64_t statTrakSwapItem1 = 0;
    std::uint64_t statTrakSwapItem2 = 0;
    Action action;
    float useTime = 0.0f;
    int stickerSlot = 0;
    std::string nameTag;
};

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
        const auto itemID = itemIDMap.getItemID(it);
        const auto newIterator = inventory.erase(it);
        if (itemID.has_value())
            responses.emplace(Response::Type::ItemRemoved, *itemID);
        return newIterator;
    }

    void updateStatTrak(std::list<inventory::Item_v2>::const_iterator it, int newStatTrak)
    {
        if (updateStatTrak(removeConstness(it), newStatTrak))
            responses.emplace(Response::Type::StatTrakUpdated, it);
    }

    void moveToFront(std::list<inventory::Item_v2>::const_iterator it)
    {
        inventory.splice(inventory.end(), inventory, it);
    }

    void assignItemID(std::list<inventory::Item_v2>::const_iterator it, std::uint64_t itemID)
    {
        itemIDMap.add(itemID, it);
    }

    [[nodiscard]] std::optional<std::list<inventory::Item_v2>::const_iterator> itemFromID(std::uint64_t itemID)
    {
        return itemIDMap.get(itemID);
    }

    [[nodiscard]] std::optional<std::uint64_t> getItemID(std::list<inventory::Item_v2>::const_iterator it)
    {
        return itemIDMap.getItemID(it);
    }

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
    ItemIDMap itemIDMap;
};

}
