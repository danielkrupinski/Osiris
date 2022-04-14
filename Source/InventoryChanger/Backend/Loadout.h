#pragma once

#include <list>
#include <optional>
#include <unordered_map>

#include "Item.h"

namespace inventory_changer::backend
{

class Loadout {
public:
    using Slot = std::uint8_t;

    void equipItemCT(ItemConstIterator index, Slot slot)
    {
        ct[slot] = index;
        ctReversed[index] = slot;
    }

    void equipItemTT(ItemConstIterator index, Slot slot)
    {
        tt[slot] = index;
        ttReversed[index] = slot;
    }

    void equipItemNoTeam(ItemConstIterator index, Slot slot)
    {
        noTeam[slot] = index;
        noTeamReversed[index] = slot;
    }

    void unequipItem(ItemConstIterator item)
    {
        eraseItemFromMaps(item, ct, ctReversed);
        eraseItemFromMaps(item, tt, ttReversed);
        eraseItemFromMaps(item, noTeam, noTeamReversed);
    }

    [[nodiscard]] std::optional<ItemConstIterator> getItemInSlotCT(Slot slot) const
    {
        if (const auto it = ct.find(slot); it != ct.end())
            return it->second;
        return std::nullopt;
    }

    [[nodiscard]] std::optional<ItemConstIterator> getItemInSlotTT(Slot slot) const
    {
        if (const auto it = tt.find(slot); it != tt.end())
            return it->second;
        return std::nullopt;
    }

    [[nodiscard]] std::optional<ItemConstIterator> getItemInSlotNoTeam(Slot slot) const
    {
        if (const auto it = noTeam.find(slot); it != noTeam.end())
            return it->second;
        return std::nullopt;
    }

private:
    struct ItemIteratorHasher {
        [[nodiscard]] std::size_t operator()(ItemConstIterator iterator) const noexcept
        {
            return std::hash<const inventory::Item*>{}(std::to_address(iterator));
        }
    };

    using SlotToItem = std::unordered_map<Slot, ItemConstIterator>;
    using ItemToSlot = std::unordered_map<ItemConstIterator, Slot, ItemIteratorHasher>;

    static void eraseItemFromMaps(ItemConstIterator item, SlotToItem& slotToItemMap, ItemToSlot& itemToSlotMap)
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
