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

    std::optional<ItemIterator> equipItemCT(ItemIterator itemIterator, Slot slot)
    {
        if (const auto it = ct.find(slot); it != ct.end()) {
            const auto previous = it->second;
            it->second = itemIterator;

            auto node = ctReversed.extract(previous);
            node.key() = itemIterator;
            ctReversed.insert(std::move(node));

            return previous;
        }

        ct.try_emplace(slot, itemIterator);
        ctReversed.try_emplace(itemIterator, slot);
        return {};
    }

    std::optional<ItemIterator> equipItemTT(ItemIterator itemIterator, Slot slot)
    {
        if (const auto it = tt.find(slot); it != tt.end()) {
            const auto previous = it->second;
            it->second = itemIterator;

            auto node = ttReversed.extract(previous);
            node.key() = itemIterator;
            ttReversed.insert(std::move(node));

            return previous;
        }

        tt.try_emplace(slot, itemIterator);
        ttReversed.try_emplace(itemIterator, slot);
        return {};
    }

    std::optional<ItemIterator> equipItemNoTeam(ItemIterator itemIterator, Slot slot)
    {
        if (const auto it = noTeam.find(slot); it != noTeam.end()) {
            const auto previous = it->second;
            it->second = itemIterator;

            auto node = noTeamReversed.extract(previous);
            node.key() = itemIterator;
            noTeamReversed.insert(std::move(node));

            return previous;
        }

        noTeam.try_emplace(slot, itemIterator);
        noTeamReversed.try_emplace(itemIterator, slot);
        return {};
    }

    void unequipItem(ItemIterator item)
    {
        eraseItemFromMaps(item, ct, ctReversed);
        eraseItemFromMaps(item, tt, ttReversed);
        eraseItemFromMaps(item, noTeam, noTeamReversed);
    }

    [[nodiscard]] std::optional<ItemIterator> getItemInSlotCT(Slot slot) const
    {
        if (const auto it = ct.find(slot); it != ct.end())
            return it->second;
        return std::nullopt;
    }

    [[nodiscard]] std::optional<ItemIterator> getItemInSlotTT(Slot slot) const
    {
        if (const auto it = tt.find(slot); it != tt.end())
            return it->second;
        return std::nullopt;
    }

    [[nodiscard]] std::optional<ItemIterator> getItemInSlotNoTeam(Slot slot) const
    {
        if (const auto it = noTeam.find(slot); it != noTeam.end())
            return it->second;
        return std::nullopt;
    }

    [[nodiscard]] std::optional<Slot> getItemEquippedSlotCT(ItemIterator itemIterator) const
    {
        if (const auto it = ctReversed.find(itemIterator); it != ctReversed.end())
            return it->second;
        return {};
    }

    [[nodiscard]] std::optional<Slot> getItemEquippedSlotTT(ItemIterator itemIterator) const
    {
        if (const auto it = ttReversed.find(itemIterator); it != ttReversed.end())
            return it->second;
        return {};
    }

    [[nodiscard]] std::optional<Slot> getItemEquippedSlotNoTeam(ItemIterator itemIterator) const
    {
        if (const auto it = noTeamReversed.find(itemIterator); it != noTeamReversed.end())
            return it->second;
        return {};
    }

private:
    using SlotToItem = std::unordered_map<Slot, ItemIterator>;
    using ItemToSlot = std::unordered_map<ItemIterator, Slot>;

    static void eraseItemFromMaps(ItemIterator item, SlotToItem& slotToItemMap, ItemToSlot& itemToSlotMap)
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
