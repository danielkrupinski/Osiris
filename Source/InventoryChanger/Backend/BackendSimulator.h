#pragma once

#include <chrono>
#include <queue>
#include <variant>

#include "Item.h"
#include "ItemIDMap.h"
#include "Loadout.h"
#include "Response.h"
#include "ResponseQueue.h"
#include "ToolUser.h"

#include <InventoryChanger/StaticData.h>

namespace inventory_changer::backend
{

struct UseToolRequest {
    enum class Action {
        None,
        Use,
        WearSticker,
        RemoveNameTag
    };

    std::uint64_t toolItemID = 0;
    std::uint64_t destItemID = 0;
    std::uint64_t statTrakSwapItem1 = 0;
    std::uint64_t statTrakSwapItem2 = 0;
    Action action = Action::None;
    int stickerSlot = 0;
    std::string nameTag;
};

class BackendSimulator {
public:
    explicit BackendSimulator(const game_items::Lookup& gameItemLookup) : gameItemLookup{ gameItemLookup } {}

    [[nodiscard]] const Loadout& getLoadout() const noexcept
    {
        return loadout;
    }

    [[nodiscard]] const ItemList& getInventory() const noexcept
    {
        return inventory;
    }

    void equipItemCT(ItemConstIterator itemIterator, Loadout::Slot slot)
    {
        loadout.equipItemCT(itemIterator, slot);
        responseQueue.add(response::ItemEquipped{ itemIterator, slot, Team::CT });
    }

    void markItemEquippedCT(ItemConstIterator itemIterator, Loadout::Slot slot)
    {
        loadout.equipItemCT(itemIterator, slot);
    }

    void equipItemTT(ItemConstIterator itemIterator, Loadout::Slot slot)
    {
        loadout.equipItemTT(itemIterator, slot);
        responseQueue.add(response::ItemEquipped{ itemIterator, slot, Team::TT });
    }

    void markItemEquippedTT(ItemConstIterator itemIterator, Loadout::Slot slot)
    {
        loadout.equipItemTT(itemIterator, slot);
    }

    void equipItemNoTeam(ItemConstIterator itemIterator, Loadout::Slot slot)
    {
        loadout.equipItemNoTeam(itemIterator, slot);
        responseQueue.add(response::ItemEquipped{ itemIterator, slot, Team::None });
    }

    void markItemEquippedNoTeam(ItemConstIterator itemIterator, Loadout::Slot slot)
    {
        loadout.equipItemNoTeam(itemIterator, slot);
    }

    [[nodiscard]] static BackendSimulator& instance()
    {
        static BackendSimulator backendSimulator{ StaticData::lookup() };
        return backendSimulator;
    }

    void clearInventory()
    {
        for (auto it = inventory.cbegin(); it != inventory.cend();)
            it = removeItem(it);
    }

    ItemConstIterator addItemUnacknowledged(inventory::Item item)
    {
        return addItem(std::move(item), true);
    }

    ItemConstIterator addItemAcknowledged(inventory::Item item)
    {
        return addItem(std::move(item), false);
    }

    ItemConstIterator removeItem(ItemConstIterator it)
    {
        const auto itemID = itemIDMap.remove(it);
        loadout.unequipItem(it);
        const auto newIterator = inventory.erase(it);
        if (itemID.has_value())
            responseQueue.add(response::ItemRemoved{ *itemID });
        return newIterator;
    }

    void updateStatTrak(ItemConstIterator it, int newStatTrak)
    {
        if (!updateStatTrak(*removeConstness(it), newStatTrak))
            return;

        if (const auto itemID = getItemID(it); itemID.has_value())
            responseQueue.add(response::StatTrakUpdated{ *itemID, newStatTrak });
    }

    void moveToFront(ItemConstIterator it)
    {
        inventory.splice(inventory.end(), inventory, it);
        if (const auto itemID = getItemID(it); itemID.has_value())
            responseQueue.add(response::ItemMovedToFront{ *itemID });
    }

    void assignItemID(ItemConstIterator it, std::uint64_t itemID)
    {
        itemIDMap.add(itemID, it);
    }

    void updateItemID(std::uint64_t oldItemID, std::uint64_t newItemID)
    {
        itemIDMap.update(oldItemID, newItemID);
    }

    [[nodiscard]] std::optional<ItemConstIterator> itemFromID(std::uint64_t itemID) const
    {
        return itemIDMap.get(itemID);
    }

    [[nodiscard]] std::optional<std::uint64_t> getItemID(ItemConstIterator it) const
    {
        return itemIDMap.getItemID(it);
    }

    void useTool(const UseToolRequest& request)
    {
        if (const auto response = processUseToolRequest(request); !response.isEmpty())
            responseQueue.add(response);
    }

    template <typename Visitor>
    void run(Visitor visitor, std::chrono::milliseconds delay)
    {
        responseQueue.visit(visitor, delay);
    }

private:
    ItemConstIterator addItem(inventory::Item item, bool asUnacknowledged)
    {
        inventory.push_back(std::move(item));
        const auto added = std::prev(inventory.end());
        responseQueue.add(response::ItemAdded{ added, asUnacknowledged });
        return added;
    }

    Response processUseToolRequest(const UseToolRequest& request)
    {
        const auto destItem = itemIDMap.get(request.destItemID);
        const auto tool = itemIDMap.get(request.toolItemID);

        if (request.action == UseToolRequest::Action::Use) {
            if (destItem.has_value() && (*destItem)->gameItem().isCase())
                return ToolUser{ *this, gameItemLookup }.openContainer(*destItem, tool);

            if (!tool.has_value())
                return {};

            if ((*tool)->gameItem().isSticker()) {
                if (!destItem.has_value())
                    return {};

                return ToolUser{ *this, gameItemLookup }.applySticker(removeConstness(*destItem), *tool, request.stickerSlot);
            } else if ((*tool)->gameItem().isOperationPass()) {
                ToolUser{ *this, gameItemLookup }.activateOperationPass(*tool);
            } else if ((*tool)->gameItem().isViewerPass()) {
                return ToolUser{ *this, gameItemLookup }.activateViewerPass(*tool);
            } else if ((*tool)->gameItem().isNameTag()) {
                if (!destItem.has_value())
                    return {};

                return ToolUser{ *this, gameItemLookup }.addNameTag(removeConstness(*destItem), *tool, request.nameTag);
            } else if ((*tool)->gameItem().isPatch()) {
                if (!destItem.has_value())
                    return {};

                return ToolUser{ *this, gameItemLookup }.applyPatch(removeConstness(*destItem), *tool, request.stickerSlot);
            } else if ((*tool)->gameItem().isGraffiti()) {
                return ToolUser{ *this, gameItemLookup }.unsealGraffiti(removeConstness(*tool));
            }
        } else if (request.action == UseToolRequest::Action::WearSticker) {
            if (!destItem.has_value())
                return {};
            return ToolUser{ *this, gameItemLookup }.wearSticker(removeConstness(*destItem), request.stickerSlot);
        }

        return {};
    }

    static bool updateStatTrak(inventory::Item& item, int newStatTrak)
    {
        if (const auto skin = item.get<inventory::Skin>()) {
            skin->statTrak = newStatTrak;
            return true;
        }

        if (const auto music = item.get<inventory::Music>()) {
            music->statTrak = newStatTrak;
            return true;
        }

        return false;
    }

    [[nodiscard]] ItemIterator removeConstness(ItemConstIterator it)
    {
        return inventory.erase(it, it);
    }

    ItemList inventory;
    Loadout loadout;
    ResponseQueue responseQueue;
    ItemIDMap itemIDMap;
    const game_items::Lookup& gameItemLookup;
};

}
