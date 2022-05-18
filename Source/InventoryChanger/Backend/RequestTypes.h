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

template <typename Tag>
struct ModifyItem {
    explicit ModifyItem(ItemConstIterator item) : item{ item } {}

    ItemConstIterator item;
};

using ActivateViewerPass = ModifyItem<struct ActivateViewerPassTag>;
using ActivateSouvenirToken = ModifyItem<struct ActivateSouvenirTokenTag>;
using RemoveNameTag = ModifyItem<struct RemoveNameTagTag>;
using UnsealGraffiti = ModifyItem<struct UnsealGraffitiTag>;
using MarkItemUpdated = ModifyItem<struct MarkItemUpdatedTag>;

struct AddNameTag {
    AddNameTag(ItemConstIterator item, ItemConstIterator nameTagItem, std::string_view nameTag)
        : item{ item }, nameTagItem{ nameTagItem }, nameTag{ nameTag } {}

    ItemConstIterator item;
    ItemConstIterator nameTagItem;
    std::string_view nameTag;
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

struct PickStickerPickEm {
    PickStickerPickEm(PickEm::PickPosition position, TournamentTeam team)
        : position{ position }, team{ team } {}

    PickEm::PickPosition position;
    TournamentTeam team;
};

}
