#pragma once

#include <cstdint>
#include <list>
#include <variant>

#include "ResponseTypes.h"

namespace inventory_changer::backend
{

using Response = std::variant<
    std::monostate,
    response::ItemAdded,
    response::ItemMovedToFront,
    response::ItemUpdated,
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
    response::StatTrakSwapped,
    response::TeamGraffitiSelected
>;

[[nodiscard]] constexpr bool isEmptyResponse(const Response& response) noexcept
{
    return std::holds_alternative<std::monostate>(response);
}

}
