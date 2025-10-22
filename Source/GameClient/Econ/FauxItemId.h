#pragma once

#include <CS2/Econ/ItemDefinitionIndex.h>
#include <CS2/Econ/ItemId.h>
#include <CS2/Econ/PaintKitIndex.h>
#include <Utils/ToUnderlying.h>

class FauxItemId {
public:
    constexpr FauxItemId(cs2::ItemDefinitionIndex itemDefinitionIndex, cs2::PaintKitIndex paintKitIndex = cs2::PaintKitIndex{}) noexcept
        : itemId{cs2::kFauxItemIdMask | toUnderlying(itemDefinitionIndex)
            | (toUnderlying(paintKitIndex) << cs2::kFauxItemIdPaintKitIndexShift)}
    {
    }

    [[nodiscard]] constexpr operator cs2::ItemId() const noexcept
    {
        return itemId;
    }

private:
    cs2::ItemId itemId;
};
