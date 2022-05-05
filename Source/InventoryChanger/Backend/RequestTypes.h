#pragma once

#include <cstdint>
#include <optional>
#include <string_view>

#include "Item.h"

namespace inventory_changer::backend::request
{

struct ApplySticker {
    ItemConstIterator item;
    ItemConstIterator sticker;
    std::uint8_t slot;
};

struct WearSticker {
    ItemConstIterator skin;
    std::uint8_t slot;
};

struct SwapStatTrak {
    ItemConstIterator itemFrom;
    ItemConstIterator itemTo;
    ItemConstIterator statTrakSwapTool;
};

struct OpenContainer {
    ItemConstIterator container;
    std::optional<ItemConstIterator> key;
};

struct ApplyPatch {
    ItemConstIterator item;
    ItemConstIterator patch;
    std::uint8_t slot;
};

struct RemovePatch {
    ItemConstIterator item;
    std::uint8_t slot;
};

struct ActivateOperationPass {
    ItemConstIterator operationPass;
};

struct ActivateViewerPass {
    ItemConstIterator viewerPass;
};

struct ActivateSouvenirToken {
    ItemConstIterator souvenirToken;
};

struct AddNameTag {
    ItemConstIterator item;
    ItemConstIterator nameTagItem;
    std::string_view nameTag;
};

struct RemoveNameTag {
    ItemConstIterator item;
};

struct UnsealGraffiti {
    ItemConstIterator item;
};

struct UpdateStatTrak {
    ItemConstIterator item;
    int newStatTrak;
};

struct SelectTeamGraffiti {
    ItemConstIterator tournamentCoin;
    std::uint16_t graffitiID;
};

}
