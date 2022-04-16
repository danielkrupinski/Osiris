#pragma once

#include <cstdint>
#include <list>
#include <variant>

#include <InventoryChanger/Inventory/Item.h>

#include "Item.h"
#include "ResponseTypes.h"

namespace inventory_changer::backend
{

struct Response {
    template <typename T>
    explicit(false) Response(const T& responseData) : data{ responseData } {}

    Response() = default;

    [[nodiscard]] constexpr bool isEmpty() const noexcept
    {
        return std::holds_alternative<std::monostate>(data);
    }

    std::variant<
        std::monostate,
        response::ItemAdded,
        response::ItemMovedToFront,
        response::ItemEquipped,
        response::ItemRemoved,
        response::StickerApplied,
        response::StickerScraped,
        response::StickerRemoved,
        response::StatTrakUpdated,
        response::ViewerPassActivated,
        response::NameTagAdded,
        response::NameTagRemoved,
        response::ContainerOpened,
        response::PatchApplied,
        response::PatchRemoved,
        response::SouvenirTokenActivated,
        response::GraffitiUnsealed,
        response::StatTrakSwapped
    > data;
};

}
