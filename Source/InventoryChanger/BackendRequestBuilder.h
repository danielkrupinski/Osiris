#pragma once

#include <cstdint>
#include <string_view>

#include "Backend/BackendSimulator.h"

namespace inventory_changer
{

class BackendRequestBuilder {
public:
    explicit BackendRequestBuilder(backend::BackendSimulator& backend) : backend{ backend } {}

    void setToolItemID(std::uint64_t itemID) noexcept
    {
        toolItemID = itemID;
    }

    void setStickerSlot(std::uint8_t slot) noexcept
    {
        stickerSlot = slot;
    }

    void setStatTrakSwapItem1(std::uint64_t itemID) noexcept
    {
        statTrakSwapItemID1 = itemID;
    }

    void setStatTrakSwapItem2(std::uint64_t itemID) noexcept
    {
        statTrakSwapItemID2 = itemID;
    }

    void setNameTag(std::string_view newName)
    {
        nameTag = newName;
    }

    void useToolOn(std::uint64_t destItemID)
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
            backend.request<backend::request::WearSticker>(*item, slot);
        else if (gameItem.isAgent())
            backend.request<backend::request::RemovePatch>(*item, slot);
    }

    void removeNameTagFrom(std::uint64_t itemID)
    {
        const auto item = backend.itemFromID(itemID);
        if (!item.has_value())
            return;

        backend.request<backend::request::RemoveNameTag>(*item);
    }

    [[nodiscard]] static BackendRequestBuilder& instance()
    {
        static BackendRequestBuilder builder{ backend::BackendSimulator::instance() };
        return builder;
    }

private:
    void useToolOnItem(backend::ItemConstIterator tool, backend::ItemConstIterator destItem)
    {
        if (tool->gameItem().isSticker() && destItem->gameItem().isSkin()) {
            backend.request<backend::request::ApplySticker>(destItem, tool, stickerSlot);
        } else if (tool->gameItem().isCaseKey() && destItem->gameItem().isCase()) {
            backend.request<backend::request::OpenContainer>(destItem, tool);
        } else if (tool->gameItem().isPatch() && destItem->gameItem().isAgent()) {
            backend.request<backend::request::ApplyPatch>(destItem, tool, stickerSlot);
        } else if (tool->gameItem().isNameTag() && destItem->gameItem().isSkin()) {
            backend.request<backend::request::AddNameTag>(destItem, tool, nameTag);
        }
    }

    void useTool(backend::ItemConstIterator tool)
    {
        if (tool->gameItem().isStatTrakSwapTool()) {
            const auto statTrakSwapItem1 = backend.itemFromID(statTrakSwapItemID1);
            const auto statTrakSwapItem2 = backend.itemFromID(statTrakSwapItemID2);

            if (statTrakSwapItem1.has_value() && statTrakSwapItem2.has_value())
                backend.request<backend::request::SwapStatTrak>(*statTrakSwapItem1, *statTrakSwapItem2, tool);
        } else if (tool->gameItem().isOperationPass()) {
            backend.request<backend::request::ActivateOperationPass>(tool);
        } else if (tool->gameItem().isViewerPass()) {
            backend.request<backend::request::ActivateViewerPass>(tool);
        } else if (tool->gameItem().isSouvenirToken()) {
            backend.request<backend::request::ActivateSouvenirToken>(tool);
        } else if (tool->gameItem().isGraffiti()) {
            backend.request<backend::request::UnsealGraffiti>(tool);
        }
    }

    void useItem(backend::ItemConstIterator item)
    {
        if (item->gameItem().isCase())
            backend.request<backend::request::OpenContainer>(item);
    }

    backend::BackendSimulator& backend;
    std::uint64_t toolItemID = 0;
    std::uint8_t stickerSlot = 0;
    std::uint64_t statTrakSwapItemID1 = 0;
    std::uint64_t statTrakSwapItemID2 = 0;
    std::string nameTag;
};

}
