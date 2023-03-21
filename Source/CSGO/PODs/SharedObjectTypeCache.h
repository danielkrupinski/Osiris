#pragma once

#include <cassert>
#include <utility>

#include <Platform/Macros/PlatformSpecific.h>
#include <CSGO/Constants/ItemId.h>
#include <Utils/Pad.h>

#include "EconItem.h"

namespace csgo
{

struct SharedObjectPOD;

struct SharedObjectTypeCachePOD {
    PAD(sizeof(void*));
    SharedObjectPOD** objects;
    PAD(WIN32_LINUX(16, 24));
    int objectCount;
    PAD(WIN32_LINUX(4, 12));
    int classID; // https://github.com/perilouswithadollarsign/cstrike15_src/blob/f82112a2388b841d72cb62ca48ab1846dfcc11c8/game/shared/econ/econ_item_constants.h#L39

    std::pair<ItemId, std::uint32_t> getHighestIDs() const noexcept
    {
        assert(classID == 1);

        ItemId maxItemID = 0;
        std::uint32_t maxInventoryID = 0;
        for (int i = 0; i < objectCount; ++i) {
            const auto item = objects[i];
            if (const auto isDefaultItem = (((EconItemPOD*)item)->itemID & 0xF000000000000000) != 0)
                continue;
            maxItemID = (std::max)(maxItemID, ((EconItemPOD*)item)->itemID);
            maxInventoryID = (std::max)(maxInventoryID, ((EconItemPOD*)item)->inventory);
        }

        return std::make_pair(maxItemID, maxInventoryID);
    }
};

}
