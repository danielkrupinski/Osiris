#pragma once

#include <cstdint>
#include <string_view>

#include <InventoryChanger/Backend/ItemIDMap.h>
#include <InventoryChanger/Backend/Response/ResponseAccumulator.h>
#include <InventoryChanger/GameItems/Lookup.h>

#include "RequestTypes.h"
#include "StorageUnitHandler.h"

namespace inventory_changer::backend
{

struct RequestBuilderParams {
    std::uint8_t stickerSlot = 0;
    ItemId statTrakSwapItemID1{};
    ItemId statTrakSwapItemID2{};
    std::string nameTag;
};

template <typename Requestor, typename StorageUnitHandler, typename XRayScannerHandler, typename ItemActivationHandler>
class RequestBuilder {
public:
    explicit RequestBuilder(const RequestBuilderParams& params, const ItemIDMap& itemIDMap, Requestor requestor, StorageUnitHandler storageUnitHandler, XRayScannerHandler xRayScannerHandler, ItemActivationHandler itemActivationHandler)
        : params{ params }, itemIDMap{ itemIDMap }, requestor{ requestor }, storageUnitHandler{ storageUnitHandler }, xRayScannerHandler{ xRayScannerHandler }, itemActivationHandler{ itemActivationHandler } {}

    void useToolOn(ItemId toolItemID, ItemId destItemID)
    {
        const auto toolItem = itemIDMap.get(toolItemID);
        const auto destItem = itemIDMap.get(destItemID);

        if (toolItem.has_value() && destItem.has_value()) {
            useToolOnItem(*toolItem, *destItem);
        } else if (toolItem.has_value()) {
            useTool(*toolItem);
        } else if (destItem.has_value()) {
            useItem(*destItem, toolItemID);
        }
    }

    void wearStickerOf(ItemId itemID, std::uint8_t slot)
    {
        const auto item = itemIDMap.get(itemID);
        if (!item.has_value())
            return;

        if (const auto gameItem = (*item)->gameItem(); gameItem.isSkin())
            request<request::WearSticker>(*item, slot);
        else if (gameItem.isAgent())
            request<request::RemovePatch>(*item, slot);
    }

    void removeNameTagFrom(ItemId itemID)
    {
        const auto item = itemIDMap.get(itemID);
        if (!item.has_value())
            return;

        request<request::RemoveNameTag>(*item);
    }

    void addToStorageUnit(ItemId itemID, ItemId storageUnitItemID)
    {
        const auto item = itemIDMap.get(itemID);
        const auto storageUnit = itemIDMap.get(storageUnitItemID);

        if (!item.has_value() || !storageUnit.has_value() || !(*storageUnit)->gameItem().isStorageUnit())
            return;

        storageUnitHandler.addItemToStorageUnit(*item, *storageUnit);
    }

    void removeFromStorageUnit(ItemId itemID, ItemId storageUnitItemID)
    {
        const auto item = itemIDMap.get(itemID);
        const auto storageUnit = itemIDMap.get(storageUnitItemID);

        if (!item.has_value() || !storageUnit.has_value() || !(*storageUnit)->gameItem().isStorageUnit())
            return;

        storageUnitHandler.removeFromStorageUnit(*item, *storageUnit);
    }

    void loadStorageUnitContents(ItemId storageUnitItemID)
    {
        const auto storageUnit = itemIDMap.get(storageUnitItemID);
        if (!storageUnit.has_value() || !(*storageUnit)->gameItem().isStorageUnit())
            return;

        storageUnitHandler.loadStorageUnitContents(*storageUnit);
    }

private:
    void useToolOnItem(ItemIterator tool, ItemIterator destItem)
    {
        if (tool->gameItem().isSticker() && destItem->gameItem().isSkin()) {
            request<request::ApplySticker>(destItem, tool, params.stickerSlot);
        } else if (tool->gameItem().isCaseKey() && destItem->gameItem().isCrate()) {
            if (destItem->getState() != inventory::Item::State::InXrayScanner)
                itemActivationHandler.openContainer(destItem, tool);
            else
                xRayScannerHandler.claimXRayScannedItem(destItem, tool);
        } else if (tool->gameItem().isPatch() && destItem->gameItem().isAgent()) {
            request<request::ApplyPatch>(destItem, tool, params.stickerSlot);
        } else if (tool->gameItem().isNameTag() && destItem->gameItem().isSkin()) {
            request<request::AddNameTag>(destItem, tool, params.nameTag);
        } else if (tool->gameItem().isCrate() && tool == destItem) {
            xRayScannerHandler.performXRayScan(tool);
        }
    }

    void useTool(ItemIterator tool)
    {
        if (tool->gameItem().isStatTrakSwapTool()) {
            const auto statTrakSwapItem1 = itemIDMap.get(params.statTrakSwapItemID1);
            const auto statTrakSwapItem2 = itemIDMap.get(params.statTrakSwapItemID2);

            if (statTrakSwapItem1.has_value() && statTrakSwapItem2.has_value())
                request<request::SwapStatTrak>(*statTrakSwapItem1, *statTrakSwapItem2, tool);
        } else if (tool->gameItem().isOperationPass()) {
            itemActivationHandler.activateOperationPass(tool);
        } else if (tool->gameItem().isViewerPass()) {
            itemActivationHandler.activateViewerPass(tool);
        } else if (tool->gameItem().isSouvenirToken()) {
            request<request::ActivateSouvenirToken>(tool);
        } else if (tool->gameItem().isGraffiti()) {
            request<request::UnsealGraffiti>(tool);
        }
    }

    void useItem(ItemIterator item, ItemId toolItemID)
    {
        constexpr auto fauxNameTagItemID = ItemId{ (std::uint64_t{ 0xF } << 60) | static_cast<std::uint16_t>(WeaponId::NameTag) };

        if (item->gameItem().isCrate()) {
            if (item->getState() != inventory::Item::State::InXrayScanner)
                itemActivationHandler.openKeylessContainer(item);
            else
                xRayScannerHandler.claimXRayScannedItemWithoutKey(item);
        } else if (item->gameItem().isStorageUnit() && toolItemID == fauxNameTagItemID) {
            storageUnitHandler.nameStorageUnit(item, params.nameTag);
        }
    }

    template <typename RequestType, typename... Args>
    void request(Args&&... args)
    {
        requestor(RequestType{ std::forward<Args>(args)... });
    }

    const RequestBuilderParams& params;
    const ItemIDMap& itemIDMap;
    Requestor requestor;
    StorageUnitHandler storageUnitHandler;
    XRayScannerHandler xRayScannerHandler;
    ItemActivationHandler itemActivationHandler;
};

}
