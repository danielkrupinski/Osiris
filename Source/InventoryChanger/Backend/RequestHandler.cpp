#include "BackendSimulator.h"
#include "RequestHandler.h"

#include <InventoryChanger/GameItems/Lookup.h>

namespace inventory_changer::backend
{

Response RequestHandler::operator()(const request::ApplySticker& request)
{
    const auto skin = constRemover.removeConstness(request.item)->get<inventory::Skin>();
    if (!skin)
        return {};

    skin->stickers[request.slot].stickerID = gameItemLookup.getStorage().getStickerKit(request.sticker->gameItem()).id;
    skin->stickers[request.slot].wear = 0.0f;

    backend.moveToFront(request.item);
    backend.removeItem(request.sticker);
    return response::StickerApplied{ request.item, request.slot };
}

Response RequestHandler::operator()(const request::WearSticker& request)
{
    const auto skin = constRemover.removeConstness(request.skin)->get<inventory::Skin>();
    if (!skin)
        return {};

    constexpr auto wearStep = 0.12f;
    const auto newWear = (skin->stickers[request.slot].wear += wearStep);

    if (const auto shouldRemove = (newWear >= 1.0f + wearStep)) {
        skin->stickers[request.slot] = {};
        return response::StickerRemoved{ request.skin, request.slot };
    }

    return response::StickerScraped{ request.skin, request.slot };
}

}
