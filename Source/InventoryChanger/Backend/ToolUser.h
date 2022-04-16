#pragma once

#include <string_view>

#include <InventoryChanger/Inventory/Item.h>

#include "Item.h"
#include "Response.h"

namespace game_items
{
    class Lookup;
}

namespace inventory_changer::backend
{

class BackendSimulator;

class ToolUser {
public:
    explicit ToolUser(BackendSimulator& backend, const game_items::Lookup& gameItemLookup) : backend{ backend }, gameItemLookup{ gameItemLookup } {}

    Response applySticker(ItemIterator item, ItemConstIterator sticker, std::uint8_t slot);
    Response applyPatch(ItemIterator item, ItemConstIterator patch, std::uint8_t slot);
    Response removePatch(ItemIterator item, std::uint8_t slot);
    void activateOperationPass(ItemConstIterator item);
    Response activateViewerPass(ItemConstIterator item);
    Response wearSticker(ItemIterator item, std::uint8_t slot);
    Response addNameTag(ItemIterator item, ItemConstIterator nameTagItem, std::string_view nameTag);
    Response removeNameTag(ItemIterator item);
    Response openContainer(ItemConstIterator item, std::optional<ItemConstIterator> key);
    Response activateSouvenirToken(ItemConstIterator item, ItemIterator tournamentCoin);
    Response unsealGraffiti(ItemIterator item);
    Response swapStatTrak(ItemIterator itemFrom, ItemIterator itemTo, ItemConstIterator statTrakSwapTool);

private:
    BackendSimulator& backend;
    const game_items::Lookup& gameItemLookup;
};

}
