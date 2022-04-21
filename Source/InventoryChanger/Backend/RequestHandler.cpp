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

}
