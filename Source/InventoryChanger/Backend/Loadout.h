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
    std::unordered_map<Slot, InventoryItemIndex> ct;
    std::unordered_map<Slot, InventoryItemIndex> tt;
    std::unordered_map<Slot, InventoryItemIndex> noTeam;
};

}
