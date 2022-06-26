#pragma once

#include <cstdint>
#include <list>
#include <variant>

#include "ResponseTypes.h"

namespace inventory_changer::backend
{

using Response = std::variant<
    std::monostate,
    response::ContainerOpened,
    response::GraffitiUnsealed,
    response::ItemAdded,
    response::ItemEquipped,
    response::ItemHidden,
    response::ItemMovedToFront,
    response::ItemRemoved,
    response::ItemUnhidden,
    response::ItemUpdated,
    response::NameTagAdded,
    response::NameTagRemoved,
    response::PatchApplied,
    response::PatchRemoved,
    response::PickEmUpdated,
    response::SouvenirTokenActivated,
    response::StatTrakSwapped,
    response::StatTrakUpdated,
    response::StickerApplied,
    response::StickerRemoved,
    response::StickerScraped,
    response::TeamGraffitiSelected,
    response::ViewerPassActivated,
    response::XRayItemClaimed,
    response::XRayScannerUsed
>;

[[nodiscard]] constexpr bool isEmptyResponse(const Response& response) noexcept
{
    return std::holds_alternative<std::monostate>(response);
}

}
