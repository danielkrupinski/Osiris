#include "BackendSimulator.h"
#include "ToolUser.h"

namespace inventory_changer::backend
{

std::optional<Response> ToolUser::applySticker(BackendSimulator& backend, std::list<inventory::Item_v2>::iterator item, std::list<inventory::Item_v2>::const_iterator sticker, std::uint8_t slot)
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

void ToolUser::activateOperationPass(BackendSimulator& backend, std::list<inventory::Item_v2>::const_iterator item)
{
    const auto& gameItem = item->gameItem();
    if (!gameItem.isOperationPass())
        return;

    const auto coinID = gameItem.getWeaponID() != WeaponId::OperationHydraPass ? static_cast<WeaponId>(static_cast<int>(gameItem.getWeaponID()) + 1) : WeaponId::BronzeOperationHydraCoin;
    if (const auto operationCoin = StaticData::lookup().findItem(coinID); operationCoin.has_value()) {
        backend.addItem(inventory::Item_v2{ *operationCoin });
        backend.removeItem(item);
    }
}

}
