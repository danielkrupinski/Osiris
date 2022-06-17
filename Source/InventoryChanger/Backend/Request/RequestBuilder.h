#pragma once

#include <cstdint>
#include <string_view>

#include <InventoryChanger/Backend/BackendSimulator.h>
#include <InventoryChanger/GameItems/Lookup.h>

namespace inventory_changer::backend
{

class RequestBuilder {
public:
    explicit RequestBuilder(BackendSimulator& backend) : backend{ backend } {}

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
        const auto toolItem = backend.itemFromID(toolItemID);
        const auto destItem = backend.itemFromID(destItemID);

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
        const auto item = backend.itemFromID(itemID);
        if (!item.has_value())
            return;

        if (const auto gameItem = (*item)->gameItem(); gameItem.isSkin())
            backend.request<request::WearSticker>(*item, slot);
        else if (gameItem.isAgent())
            backend.request<request::RemovePatch>(*item, slot);
    }

    void removeNameTagFrom(std::uint64_t itemID)
    {
        const auto item = backend.itemFromID(itemID);
        if (!item.has_value())
            return;

        backend.request<request::RemoveNameTag>(*item);
    }

    void placePickEmPick(std::uint16_t group, std::uint8_t indexInGroup, int stickerID)
    {
        const auto& gameItemLookup = backend.getGameItemLookup();

        const auto sticker = gameItemLookup.findSticker(stickerID);
        if (!sticker || !sticker->get().isSticker())
            return;

        const auto tournamentTeam = gameItemLookup.getStorage().getStickerKit(*sticker).tournamentTeam;
        backend.request<request::PickStickerPickEm>(PickEm::PickPosition{ 19, group, indexInGroup }, tournamentTeam);
    }

private:
    void useToolOnItem(backend::ItemIterator tool, backend::ItemIterator destItem)
    {
        if (tool->gameItem().isSticker() && destItem->gameItem().isSkin()) {
            backend.request<request::ApplySticker>(destItem, tool, stickerSlot);
        } else if (tool->gameItem().isCaseKey() && destItem->gameItem().isCase()) {
            if (!destItem->isHidden())
                backend.request<request::OpenContainer>(destItem, tool);
            else
                backend.request<request::ClaimXRayScannedItem>(destItem, tool);
        } else if (tool->gameItem().isPatch() && destItem->gameItem().isAgent()) {
            backend.request<request::ApplyPatch>(destItem, tool, stickerSlot);
        } else if (tool->gameItem().isNameTag() && destItem->gameItem().isSkin()) {
            backend.request<request::AddNameTag>(destItem, tool, nameTag);
        } else if (tool->gameItem().isCase() && tool == destItem) {
            backend.request<request::PerformXRayScan>(tool);
        }
    }

    void useTool(backend::ItemIterator tool)
    {
        if (tool->gameItem().isStatTrakSwapTool()) {
            const auto statTrakSwapItem1 = backend.itemFromID(statTrakSwapItemID1);
            const auto statTrakSwapItem2 = backend.itemFromID(statTrakSwapItemID2);

            if (statTrakSwapItem1.has_value() && statTrakSwapItem2.has_value())
                backend.request<request::SwapStatTrak>(*statTrakSwapItem1, *statTrakSwapItem2, tool);
        } else if (tool->gameItem().isOperationPass()) {
            backend.request<request::ActivateOperationPass>(tool);
        } else if (tool->gameItem().isViewerPass()) {
            backend.request<request::ActivateViewerPass>(tool);
        } else if (tool->gameItem().isSouvenirToken()) {
            backend.request<request::ActivateSouvenirToken>(tool);
        } else if (tool->gameItem().isGraffiti()) {
            backend.request<request::UnsealGraffiti>(tool);
        }
    }

    void useItem(backend::ItemIterator item)
    {
        if (item->gameItem().isCase()) {
            if (!item->isHidden())
                backend.request<request::OpenContainer>(item);
            else
                backend.request<request::ClaimXRayScannedItem>(item);
        }
    }

    BackendSimulator& backend;
    std::uint8_t stickerSlot = 0;
    std::uint64_t statTrakSwapItemID1 = 0;
    std::uint64_t statTrakSwapItemID2 = 0;
    std::string nameTag;
};

}
