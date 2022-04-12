#pragma once

#include <list>
#include <optional>
#include <unordered_map>

namespace inventory_changer::backend
{

class Loadout {
public:
    using InventoryItemIndex = std::list<inventory::Item>::const_iterator;
    using Slot = std::uint8_t;

    void equipItemCT(InventoryItemIndex index, Slot slot)
    {
        ct[slot] = index;
    }

    void equipItemTT(InventoryItemIndex index, Slot slot)
    {
        tt[slot] = index;
    }

    void equipItemNoTeam(InventoryItemIndex index, Slot slot)
    {
        noTeam[slot] = index;
    }

    void unequipItem(InventoryItemIndex item)
    {
        eraseItemFromMaps(item, ct, ctReversed);
        eraseItemFromMaps(item, tt, ttReversed);
        eraseItemFromMaps(item, noTeam, noTeamReversed);
    }

    [[nodiscard]] std::optional<InventoryItemIndex> getItemInSlotCT(Slot slot) const
    {
        if (const auto it = ct.find(slot); it != ct.end())
            return it->second;
        return std::nullopt;
    }

    [[nodiscard]] std::optional<InventoryItemIndex> getItemInSlotTT(Slot slot) const
    {
        if (const auto it = tt.find(slot); it != tt.end())
            return it->second;
        return std::nullopt;
    }

    [[nodiscard]] std::optional<InventoryItemIndex> getItemInSlotNoTeam(Slot slot) const
    {
        if (const auto it = noTeam.find(slot); it != noTeam.end())
            return it->second;
        return std::nullopt;
    }

private:
    struct ItemIteratorHasher {
        [[nodiscard]] std::size_t operator()(std::list<inventory::Item>::const_iterator iterator) const noexcept
        {
            return std::hash<const inventory::Item*>{}(std::to_address(iterator));
        }
    };

    using SlotToItem = std::unordered_map<Slot, InventoryItemIndex>;
    using ItemToSlot = std::unordered_map<InventoryItemIndex, Slot, ItemIteratorHasher>;

    static void eraseItemFromMaps(InventoryItemIndex item, SlotToItem& slotToItemMap, ItemToSlot& itemToSlotMap)
    {
        if (const auto it = itemToSlotMap.find(item); it != itemToSlotMap.end()) {
            slotToItemMap.erase(it->second);
            itemToSlotMap.erase(it);
        }
    }

    SlotToItem ct;
    ItemToSlot ctReversed;
    SlotToItem tt;
    ItemToSlot ttReversed;
    SlotToItem noTeam;
    ItemToSlot noTeamReversed;
};

}
