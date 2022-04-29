#pragma once

#include <variant>

#include "Item.h"
#include "ResponseTypes.h"

namespace inventory_changer::backend
{

struct ItemInResponse {
    explicit ItemInResponse(ItemConstIterator item) : item{ item } {}

    void operator()(std::monostate) const;
    void operator()(const response::ItemAdded& response) const;
    void operator()(const response::ItemMovedToFront& response) const;
    void operator()(const response::ItemEquipped& response) const;
    void operator()(const response::ItemRemoved& response) const;
    void operator()(const response::StickerApplied& response) const;
    void operator()(const response::StickerScraped& response) const;
    void operator()(const response::StickerRemoved& response) const;
    void operator()(const response::StatTrakUpdated& response) const;
    void operator()(const response::ViewerPassActivated& response) const;
    void operator()(const response::NameTagAdded& response) const;
    void operator()(const response::NameTagRemoved& response) const;
    void operator()(const response::ContainerOpened& response) const;
    void operator()(const response::PatchApplied& response) const;
    void operator()(const response::PatchRemoved& response) const;
    void operator()(const response::SouvenirTokenActivated& response) const;
    void operator()(const response::GraffitiUnsealed& response) const;
    void operator()(const response::StatTrakSwapped& response) const;

private:
    ItemConstIterator item;
};

}
