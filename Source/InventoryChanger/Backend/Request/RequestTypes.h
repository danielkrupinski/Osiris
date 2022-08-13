#pragma once

#include <cstdint>
#include <optional>
#include <string_view>

#include <InventoryChanger/Backend/Item.h>
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

struct OpenContainer {
    explicit OpenContainer(ItemIterator container) : container{ container } {}
    OpenContainer(ItemIterator container, ItemIterator key)
        : container{ container }, key{ key } {}

    ItemIterator container;
    std::optional<ItemIterator> key;
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

struct ActivateOperationPass {
    explicit ActivateOperationPass(ItemIterator operationPass) : operationPass{ operationPass } {}

    ItemIterator operationPass;
};

template <typename Tag>
struct ModifyItem {
    explicit ModifyItem(ItemIterator item) : item{ item } {}

    ItemIterator item;
};

using ActivateViewerPass = ModifyItem<struct ActivateViewerPassTag>;
using ActivateSouvenirToken = ModifyItem<struct ActivateSouvenirTokenTag>;
using RemoveNameTag = ModifyItem<struct RemoveNameTagTag>;
using UnsealGraffiti = ModifyItem<struct UnsealGraffitiTag>;
using MarkItemUpdated = ModifyItem<struct MarkItemUpdatedTag>;
using HideItem = ModifyItem<struct HideItemTag>;
using UnhideItem = ModifyItem<struct UnhideItemTag>;

struct AddNameTag {
    AddNameTag(ItemIterator item, ItemIterator nameTagItem, std::string_view nameTag)
        : item{ item }, nameTagItem{ nameTagItem }, nameTag{ nameTag } {}

    ItemIterator item;
    ItemIterator nameTagItem;
    std::string_view nameTag;
};

struct NameStorageUnit {
    NameStorageUnit(ItemIterator storageUnit, std::string_view name)
        : storageUnit{ storageUnit }, name{ name } {}

    ItemIterator storageUnit;
    std::string_view name;
};

struct UpdateStatTrak {
    UpdateStatTrak(ItemIterator item, int newStatTrak)
        : item{ item }, newStatTrak{ newStatTrak } {}

    ItemIterator item;
    int newStatTrak;
};

struct SelectTeamGraffiti {
    SelectTeamGraffiti(ItemIterator tournamentCoin, std::uint16_t graffitiID)
        : tournamentCoin{ tournamentCoin }, graffitiID{ graffitiID } {}

    ItemIterator tournamentCoin;
    std::uint16_t graffitiID;
};

struct PickStickerPickEm {
    PickStickerPickEm(PickEm::PickPosition position, csgo::TournamentTeam team)
        : position{ position }, team{ team } {}

    PickEm::PickPosition position;
    csgo::TournamentTeam team;
};

struct PerformXRayScan {
    explicit PerformXRayScan(ItemIterator crate) : crate{ crate } {}

    ItemIterator crate;
};

struct ClaimXRayScannedItem {
    explicit ClaimXRayScannedItem(ItemIterator container) : container{ container } {}
    ClaimXRayScannedItem(ItemIterator container, ItemIterator key)
        : container{ container }, key{ key } {}

    ItemIterator container;
    std::optional<ItemIterator> key;
};

struct BindItemToStorageUnit {
    explicit BindItemToStorageUnit(ItemIterator item, ItemIterator storageUnit)
        : item{ item }, storageUnit{ storageUnit } {}

    ItemIterator item;
    ItemIterator storageUnit;
};

struct AddItemToStorageUnit {
    explicit AddItemToStorageUnit(ItemIterator item, ItemIterator storageUnit)
        : item{ item }, storageUnit{ storageUnit } {}

    ItemIterator item;
    ItemIterator storageUnit;
};

struct RemoveFromStorageUnit {
    explicit RemoveFromStorageUnit(ItemIterator item, ItemIterator storageUnit)
        : item{ item }, storageUnit{ storageUnit } {}

    ItemIterator item;
    ItemIterator storageUnit;
};

struct MarkStorageUnitModified {
    explicit MarkStorageUnitModified(ItemIterator storageUnit)
        : storageUnit{ storageUnit } {}

    ItemIterator storageUnit;
};

struct UpdateStorageUnitAttributes {
    explicit UpdateStorageUnitAttributes(ItemIterator storageUnit)
        : storageUnit{ storageUnit } {}

    ItemIterator storageUnit;
};

}
