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
            useItem(*destItem);
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

private:
    void useToolOnItem(backend::ItemIterator tool, backend::ItemIterator destItem)
    {
        if (tool->gameItem().isSticker() && destItem->gameItem().isSkin()) {
            request<request::ApplySticker>(destItem, tool, stickerSlot);
        } else if (tool->gameItem().isCaseKey() && destItem->gameItem().isCase()) {
            if (!destItem->isHidden())
                request<request::OpenContainer>(destItem, tool);
            else
                request<request::ClaimXRayScannedItem>(destItem, tool);
        } else if (tool->gameItem().isPatch() && destItem->gameItem().isAgent()) {
            request<request::ApplyPatch>(destItem, tool, stickerSlot);
        } else if (tool->gameItem().isNameTag() && destItem->gameItem().isSkin()) {
            request<request::AddNameTag>(destItem, tool, nameTag);
        } else if (tool->gameItem().isCase() && tool == destItem) {
            request<request::PerformXRayScan>(tool);
        }
    }

    void useTool(backend::ItemIterator tool)
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

    void useItem(backend::ItemIterator item)
    {
        if (item->gameItem().isCase()) {
            if (!item->isHidden())
                request<request::OpenContainer>(item);
            else
                request<request::ClaimXRayScannedItem>(item);
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
