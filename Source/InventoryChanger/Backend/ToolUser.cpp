#include "BackendSimulator.h"
#include "ToolUser.h"

#include <InventoryChanger/ItemGenerator.h>
#include <InventoryChanger/GameItems/Lookup.h>

namespace inventory_changer::backend
{

Response ToolUser::applySticker(ItemIterator item, ItemConstIterator sticker, std::uint8_t slot)
{
    const auto skin = item->get<inventory::Skin>();
    if (!skin)
        return {};

    skin->stickers[slot].stickerID = gameItemLookup.getStorage().getStickerKit(sticker->gameItem()).id;
    skin->stickers[slot].wear = 0.0f;

    backend.moveToFront(item);
    backend.removeItem(sticker);
    return response::StickerApplied{ item, slot };
}

Response ToolUser::applyPatch(ItemIterator item, ItemConstIterator patch, std::uint8_t slot)
{
    const auto agent = item->getOrCreate<inventory::Agent>();
    if (!agent)
        return {};

    agent->patches[slot].patchID = gameItemLookup.getStorage().getPatch(patch->gameItem()).id;
    backend.moveToFront(item);
    backend.removeItem(patch);
    return response::PatchApplied{ item, slot };
}

Response ToolUser::removePatch(ItemIterator item, std::uint8_t slot)
{
    const auto agent = item->get<inventory::Agent>();
    if (!agent)
        return {};

    agent->patches[slot].patchID = 0;
    backend.moveToFront(item);
    return response::PatchRemoved{ item, slot };
}

void ToolUser::activateOperationPass(ItemConstIterator item)
{
    const auto& gameItem = item->gameItem();
    if (!gameItem.isOperationPass())
        return;

    const auto coinID = gameItem.getWeaponID() != WeaponId::OperationHydraPass ? static_cast<WeaponId>(static_cast<int>(gameItem.getWeaponID()) + 1) : WeaponId::BronzeOperationHydraCoin;
    if (const auto operationCoin = gameItemLookup.findItem(coinID); operationCoin.has_value()) {
        backend.addItemUnacknowledged(inventory::Item{ *operationCoin });
        backend.removeItem(item);
    }
}

Response ToolUser::activateViewerPass(ItemConstIterator item)
{
    const auto& gameItem = item->gameItem();
    if (!gameItem.isViewerPass())
        return {};

    const auto coinID = static_cast<WeaponId>(static_cast<int>(gameItem.getWeaponID()) + 1);
    if (const auto eventCoin = gameItemLookup.findItem(coinID); eventCoin.has_value()) {
        const auto addedEventCoin = backend.addItemUnacknowledged(inventory::Item{ *eventCoin });
        backend.removeItem(item);
        return response::ViewerPassActivated{ addedEventCoin };
    }
    return {};
}

Response ToolUser::wearSticker(ItemIterator item, std::uint8_t slot)
{
    const auto skin = item->get<inventory::Skin>();
    if (!skin)
        return {};

    constexpr auto wearStep = 0.12f;
    const auto newWear = (skin->stickers[slot].wear += wearStep);

    if (const auto shouldRemove = (newWear >= 1.0f + wearStep)) {
        skin->stickers[slot] = {};
        return response::StickerRemoved{ item, slot };
    }

    return response::StickerScraped{ item, slot };
}

Response ToolUser::addNameTag(ItemIterator item, ItemConstIterator nameTagItem, std::string_view nameTag)
{
    const auto skin = item->get<inventory::Skin>();
    if (!skin)
        return {};

    skin->nameTag = nameTag;
    backend.removeItem(nameTagItem);
    backend.moveToFront(item);
    return response::NameTagAdded{ item };
}

Response ToolUser::removeNameTag(ItemIterator item)
{
    if (const auto skin = item->get<inventory::Skin>()) {
        skin->nameTag.clear();
        backend.moveToFront(item);
        return response::NameTagRemoved{ item };
    }
    return {};
}

Response ToolUser::openContainer(ItemConstIterator container, std::optional<ItemConstIterator> key)
{
    if (!container->gameItem().isCase())
        return {};

    if (key.has_value()) {
        if (const auto& keyItem = *key; keyItem->gameItem().isCaseKey())
            backend.removeItem(keyItem);
    }

    auto generatedItem = ItemGenerator::generateItemFromContainer(*container);
    backend.removeItem(container);
    const auto receivedItem = backend.addItemUnacknowledged(std::move(generatedItem));
    return response::ContainerOpened{ receivedItem };
}

Response ToolUser::activateSouvenirToken(ItemConstIterator item, ItemIterator tournamentCoin)
{
    if (!item->gameItem().isSouvenirToken())
        return {};

    const auto tournamentCoinData = tournamentCoin->get<inventory::TournamentCoin>();
    if (!tournamentCoinData)
        return {};

    ++tournamentCoinData->dropsAwarded;
    backend.removeItem(item);
    return response::SouvenirTokenActivated{ tournamentCoin };
}

Response ToolUser::unsealGraffiti(ItemIterator item)
{
    if (!item->gameItem().isGraffiti())
        return {};

    const auto graffiti = item->getOrCreate<inventory::Graffiti>();
    if (!graffiti)
        return {};

    graffiti->usesLeft = 50;

    backend.moveToFront(item);
    return response::GraffitiUnsealed{ item };
}

Response ToolUser::swapStatTrak(ItemIterator itemFrom, ItemIterator itemTo, ItemConstIterator statTrakSwapTool)
{
    if (!(itemFrom->gameItem().isSkin() && itemTo->gameItem().isSkin() && statTrakSwapTool->gameItem().isStatTrakSwapTool()))
        return {};

    const auto skinFrom = itemFrom->get<inventory::Skin>();
    if (!skinFrom)
        return {};

    const auto skinTo = itemFrom->get<inventory::Skin>();
    if (!skinTo)
        return {};

    std::swap(skinFrom->statTrak, skinTo->statTrak);
    backend.removeItem(statTrakSwapTool);
    backend.moveToFront(itemFrom);
    backend.moveToFront(itemTo);
    return response::StatTrakSwapped{ itemFrom, itemTo };
}

}
