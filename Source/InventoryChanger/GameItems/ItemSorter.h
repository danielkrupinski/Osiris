#pragma once

#include "Item.h"
#include "Storage.h"

namespace game_items
{

struct ItemSorter {
    explicit ItemSorter(const Storage& storage) : storage{ storage } {}

    [[nodiscard]] bool operator()(const Item& a, const Item& b) const
    {
        if (a.getWeaponID() == b.getWeaponID() && (a.isSkin() || a.isGloves()) && (b.isSkin() || b.isGloves()))
            return storage.getPaintKit(a).id < storage.getPaintKit(b).id;
        return a.getWeaponID() < b.getWeaponID();
    }

private:
    const Storage& storage;
};

}
