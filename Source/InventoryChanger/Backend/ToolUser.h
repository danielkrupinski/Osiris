#pragma once

#include <optional>

#include <InventoryChanger/Inventory/Item.h>
#include <InventoryChanger/StaticData.h>
#include <InventoryChanger/GameItems/Lookup.h>

#include "BackendSimulator.h"
#include "Response.h"

namespace inventory_changer::backend
{

std::optional<Response> applySticker(BackendSimulator backend, std::list<inventory::Item_v2>::iterator item, std::list<inventory::Item_v2>::const_iterator sticker, std::uint8_t slot)
{
    const auto skin = item->get<inventory::Skin>();
    if (!skin)
        return {};

    skin->stickers[slot].stickerID = StaticData::lookup().getStorage().getStickerKit(sticker->gameItem()).id;
    skin->stickers[slot].wear = 0.0f;

    backend.moveToFront(item);
    backend.removeItem(sticker);
    return Response{ Response::Type::StickerApplied, Response::StickerApplied{ item, slot } };
}

}
