#pragma once

#include <cassert>
#include <utility>

#include <Platform/PlatformSpecific.h>
#include <SDK/Constants/ItemId.h>
#include <SDK/Pad.h>

#include "EconItem.h"

namespace csgo::pod
{

struct SharedObject;

struct SharedObjectTypeCache {
    PAD(sizeof(void*))
    csgo::pod::SharedObject** objects;
    PAD(WIN32_LINUX(16, 24))
    int objectCount;
    PAD(WIN32_LINUX(4, 12))
    int classID; // https://github.com/perilouswithadollarsign/cstrike15_src/blob/f82112a2388b841d72cb62ca48ab1846dfcc11c8/game/shared/econ/econ_item_constants.h#L39

    std::pair<csgo::ItemId, std::uint32_t> getHighestIDs() const noexcept
    {
        assert(classID == 1);

        csgo::ItemId maxItemID = 0;
        std::uint32_t maxInventoryID = 0;
        for (int i = 0; i < objectCount; ++i) {
            const auto item = objects[i];
            if (const auto isDefaultItem = (((csgo::pod::EconItem*)item)->itemID & 0xF000000000000000) != 0)
                continue;
            maxItemID = (std::max)(maxItemID, ((csgo::pod::EconItem*)item)->itemID);
            maxInventoryID = (std::max)(maxInventoryID, ((csgo::pod::EconItem*)item)->inventory);
        }

        return std::make_pair(maxItemID, maxInventoryID);
    }
};

}
