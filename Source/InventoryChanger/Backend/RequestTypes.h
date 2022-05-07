#pragma once

#include <cstdint>
#include <optional>
#include <string_view>

#include "Item.h"

namespace inventory_changer::backend::request
{

struct ApplySticker {
    ApplySticker(ItemConstIterator item, ItemConstIterator sticker, std::uint8_t slot)
        : item{ item }, sticker{ sticker }, slot{ slot } {}

    ItemConstIterator item;
    ItemConstIterator sticker;
    std::uint8_t slot;
};

struct WearSticker {
    WearSticker(ItemConstIterator skin, std::uint8_t slot) : skin{ skin }, slot{ slot } {}

    ItemConstIterator skin;
    std::uint8_t slot;
};

struct SwapStatTrak {
    SwapStatTrak(ItemConstIterator itemFrom, ItemConstIterator itemTo, ItemConstIterator statTrakSwapTool)
        : itemFrom{ itemFrom }, itemTo{ itemTo }, statTrakSwapTool{ statTrakSwapTool } {}

    ItemConstIterator itemFrom;
    ItemConstIterator itemTo;
    ItemConstIterator statTrakSwapTool;
};

struct OpenContainer {
    explicit OpenContainer(ItemConstIterator container) : container{ container } {}
    OpenContainer(ItemConstIterator container, ItemConstIterator key)
        : container{ container }, key{ key } {}

    ItemConstIterator container;
    std::optional<ItemConstIterator> key;
};

struct ApplyPatch {
    ApplyPatch(ItemConstIterator item, ItemConstIterator patch, std::uint8_t slot)
        : item{ item }, patch{ patch }, slot{ slot } {}

    ItemConstIterator item;
    ItemConstIterator patch;
    std::uint8_t slot;
};

struct RemovePatch {
    RemovePatch(ItemConstIterator item, std::uint8_t slot) : item{ item }, slot{ slot } {}

    ItemConstIterator item;
    std::uint8_t slot;
};

struct ActivateOperationPass {
    explicit ActivateOperationPass(ItemConstIterator operationPass) : operationPass{ operationPass } {}

    ItemConstIterator operationPass;
};

struct ActivateViewerPass {
    explicit ActivateViewerPass(ItemConstIterator viewerPass) : viewerPass{ viewerPass } {}

    ItemConstIterator viewerPass;
};

struct ActivateSouvenirToken {
    explicit ActivateSouvenirToken(ItemConstIterator souvenirToken) : souvenirToken{ souvenirToken } {}

    ItemConstIterator souvenirToken;
};

struct AddNameTag {
    AddNameTag(ItemConstIterator item, ItemConstIterator nameTagItem, std::string_view nameTag)
        : item{ item }, nameTagItem{ nameTagItem }, nameTag{ nameTag } {}

    ItemConstIterator item;
    ItemConstIterator nameTagItem;
    std::string_view nameTag;
};

struct RemoveNameTag {
    explicit RemoveNameTag(ItemConstIterator item) : item{ item } {}

    ItemConstIterator item;
};

struct UnsealGraffiti {
    explicit UnsealGraffiti(ItemConstIterator item) : item{ item } {}

    ItemConstIterator item;
};

struct UpdateStatTrak {
    UpdateStatTrak(ItemConstIterator item, int newStatTrak)
        : item{ item }, newStatTrak{ newStatTrak } {}

    ItemConstIterator item;
    int newStatTrak;
};

struct SelectTeamGraffiti {
    SelectTeamGraffiti(ItemConstIterator tournamentCoin, std::uint16_t graffitiID)
        : tournamentCoin{ tournamentCoin }, graffitiID{ graffitiID } {}

    ItemConstIterator tournamentCoin;
    std::uint16_t graffitiID;
};

}
