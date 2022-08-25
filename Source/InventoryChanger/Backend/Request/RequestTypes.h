#pragma once

#include <cstdint>
#include <optional>
#include <string_view>

#include <InventoryChanger/Backend/Item.h>
#include <InventoryChanger/Backend/Loadout.h>
#include <InventoryChanger/Backend/PickEm.h>

namespace inventory_changer::backend::request
{

struct ApplySticker {
    ApplySticker(ItemIterator item, ItemIterator sticker, std::uint8_t slot)
        : item{ item }, sticker{ sticker }, slot{ slot } {}

    ItemIterator item;
    ItemIterator sticker;
    std::uint8_t slot;
};

struct WearSticker {
    WearSticker(ItemIterator skin, std::uint8_t slot) : skin{ skin }, slot{ slot } {}

    ItemIterator skin;
    std::uint8_t slot;
};

struct SwapStatTrak {
    SwapStatTrak(ItemIterator itemFrom, ItemIterator itemTo, ItemIterator statTrakSwapTool)
        : itemFrom{ itemFrom }, itemTo{ itemTo }, statTrakSwapTool{ statTrakSwapTool } {}

    ItemIterator itemFrom;
    ItemIterator itemTo;
    ItemIterator statTrakSwapTool;
};

struct ApplyPatch {
    ApplyPatch(ItemIterator item, ItemIterator patch, std::uint8_t slot)
        : item{ item }, patch{ patch }, slot{ slot } {}

    ItemIterator item;
    ItemIterator patch;
    std::uint8_t slot;
};

struct RemovePatch {
    RemovePatch(ItemIterator item, std::uint8_t slot) : item{ item }, slot{ slot } {}

    ItemIterator item;
    std::uint8_t slot;
};

template <typename Tag>
struct ModifyItem {
    explicit ModifyItem(ItemIterator item) : item{ item } {}

    ItemIterator item;
};

using ActivateSouvenirToken = ModifyItem<struct ActivateSouvenirTokenTag>;
using RemoveNameTag = ModifyItem<struct RemoveNameTagTag>;
using UnsealGraffiti = ModifyItem<struct UnsealGraffitiTag>;

struct AddNameTag {
    AddNameTag(ItemIterator item, ItemIterator nameTagItem, std::string_view nameTag)
        : item{ item }, nameTagItem{ nameTagItem }, nameTag{ nameTag } {}

    ItemIterator item;
    ItemIterator nameTagItem;
    std::string_view nameTag;
};

}
