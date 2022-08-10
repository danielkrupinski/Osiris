#pragma once

#include <cstdint>
#include <string_view>

#include <InventoryChanger/Backend/ItemIDMap.h>
#include <InventoryChanger/GameItems/Lookup.h>

#include "RequestTypes.h"

namespace inventory_changer::backend
{

template <typename Requestor>
class RequestBuilder {
public:
    explicit RequestBuilder(const ItemIDMap& itemIDMap, Requestor requestor) : itemIDMap{ itemIDMap }, requestor{ requestor } {}

    void setStickerSlot(std::uint8_t slot) noexcept
    {
        stickerSlot = slot;
    }

    void setStatTrakSwapItems(std::uint64_t itemId1, std::uint64_t itemId2) noexcept
    {
        statTrakSwapItemID1 = itemId1;
        statTrakSwapItemID2 = itemId2;
    }

    void setNameTag(std::string_view newName)
    {
        nameTag = newName;
    }

    void useToolOn(std::uint64_t toolItemID, std::uint64_t destItemID)
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

    void wearStickerOf(std::uint64_t itemID, std::uint8_t slot)
    {
        const auto item = itemIDMap.get(itemID);
        if (!item.has_value())
            return;

        if (const auto gameItem = (*item)->gameItem(); gameItem.isSkin())
            request<request::WearSticker>(*item, slot);
        else if (gameItem.isAgent())
            request<request::RemovePatch>(*item, slot);
    }

    void removeNameTagFrom(std::uint64_t itemID)
    {
        const auto item = itemIDMap.get(itemID);
        if (!item.has_value())
            return;

        request<request::RemoveNameTag>(*item);
    }

    void addToStorageUnit(std::uint64_t itemID, std::uint64_t storageUnitItemID)
    {
        const auto item = itemIDMap.get(itemID);
        const auto storageUnit = itemIDMap.get(storageUnitItemID);

        if (!item.has_value() || !storageUnit.has_value() || !(*storageUnit)->gameItem().isStorageUnit())
            return;

        request<request::AddItemToStorageUnit>(*item, *storageUnit);
    }

    void removeFromStorageUnit(std::uint64_t itemID, std::uint64_t storageUnitItemID)
    {
        const auto item = itemIDMap.get(itemID);
        const auto storageUnit = itemIDMap.get(storageUnitItemID);

        if (!item.has_value() || !storageUnit.has_value() || !(*storageUnit)->gameItem().isStorageUnit())
            return;

        request<request::RemoveFromStorageUnit>(*item, *storageUnit);
    }

private:
    void useToolOnItem(ItemIterator tool, ItemIterator destItem)
    {
        if (tool->gameItem().isSticker() && destItem->gameItem().isSkin()) {
            request<request::ApplySticker>(destItem, tool, stickerSlot);
        } else if (tool->gameItem().isCaseKey() && destItem->gameItem().isCrate()) {
            if (destItem->getState() != inventory::Item::State::InXrayScanner)
                request<request::OpenContainer>(destItem, tool);
            else
                request<request::ClaimXRayScannedItem>(destItem, tool);
        } else if (tool->gameItem().isPatch() && destItem->gameItem().isAgent()) {
            request<request::ApplyPatch>(destItem, tool, stickerSlot);
        } else if (tool->gameItem().isNameTag() && destItem->gameItem().isSkin()) {
            request<request::AddNameTag>(destItem, tool, nameTag);
        } else if (tool->gameItem().isCrate() && tool == destItem) {
            request<request::PerformXRayScan>(tool);
        }
    }

    void useTool(ItemIterator tool)
    {
        if (tool->gameItem().isStatTrakSwapTool()) {
            const auto statTrakSwapItem1 = itemIDMap.get(statTrakSwapItemID1);
            const auto statTrakSwapItem2 = itemIDMap.get(statTrakSwapItemID2);

            if (statTrakSwapItem1.has_value() && statTrakSwapItem2.has_value())
                request<request::SwapStatTrak>(*statTrakSwapItem1, *statTrakSwapItem2, tool);
        } else if (tool->gameItem().isOperationPass()) {
            request<request::ActivateOperationPass>(tool);
        } else if (tool->gameItem().isViewerPass()) {
            request<request::ActivateViewerPass>(tool);
        } else if (tool->gameItem().isSouvenirToken()) {
            request<request::ActivateSouvenirToken>(tool);
        } else if (tool->gameItem().isGraffiti()) {
            request<request::UnsealGraffiti>(tool);
        }
    }

    void useItem(ItemIterator item, std::uint64_t toolItemID)
    {
        constexpr std::uint64_t fauxNameTagItemID = (std::uint64_t{ 0xF } << 60) | static_cast<std::uint16_t>(WeaponId::NameTag);

        if (item->gameItem().isCrate()) {
            if (item->getState() != inventory::Item::State::InXrayScanner)
                request<request::OpenContainer>(item);
            else
                request<request::ClaimXRayScannedItem>(item);
        } else if (item->gameItem().isStorageUnit() && toolItemID == fauxNameTagItemID) {
            request<request::NameStorageUnit>(item, nameTag);
        }
    }

    template <typename RequestType, typename... Args>
    void request(Args&&... args)
    {
        requestor.template request<RequestType>(std::forward<Args>(args)...);
    }

    const ItemIDMap& itemIDMap;
    Requestor requestor;
    std::uint8_t stickerSlot = 0;
    std::uint64_t statTrakSwapItemID1 = 0;
    std::uint64_t statTrakSwapItemID2 = 0;
    std::string nameTag;
};

}
