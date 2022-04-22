#include "BackendSimulator.h"
#include "RequestHandler.h"

#include <InventoryChanger/GameItems/Lookup.h>
#include <InventoryChanger/ItemGenerator.h>

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

Response RequestHandler::operator()(const request::SwapStatTrak& request)
{
    if (!(request.itemFrom->gameItem().isSkin() && request.itemTo->gameItem().isSkin() && request.statTrakSwapTool->gameItem().isStatTrakSwapTool()))
        return {};

    const auto skinFrom = constRemover.removeConstness(request.itemFrom)->get<inventory::Skin>();
    if (!skinFrom)
        return {};

    const auto skinTo = constRemover.removeConstness(request.itemFrom)->get<inventory::Skin>();
    if (!skinTo)
        return {};

    std::swap(skinFrom->statTrak, skinTo->statTrak);
    backend.removeItem(request.statTrakSwapTool);
    backend.moveToFront(request.itemFrom);
    backend.moveToFront(request.itemTo);
    return response::StatTrakSwapped{ request.itemFrom, request.itemTo };
}

Response RequestHandler::operator()(const request::OpenContainer& request)
{
    if (!request.container->gameItem().isCase())
        return {};

    if (request.key.has_value()) {
        if (const auto& keyItem = *request.key; keyItem->gameItem().isCaseKey())
            backend.removeItem(keyItem);
    }

    auto generatedItem = ItemGenerator::generateItemFromContainer(*request.container);
    backend.removeItem(request.container);
    const auto receivedItem = backend.addItemUnacknowledged(std::move(generatedItem));
    return response::ContainerOpened{ receivedItem };
}

}
