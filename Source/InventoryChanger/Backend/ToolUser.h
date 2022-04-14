#pragma once

#include <optional>
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

    std::optional<Response> applySticker(ItemIterator item, ItemConstIterator sticker, std::uint8_t slot);
    std::optional<Response> applyPatch(ItemIterator item, ItemConstIterator patch, std::uint8_t slot);
    std::optional<Response> removePatch(ItemIterator item, std::uint8_t slot);
    void activateOperationPass(ItemConstIterator item);
    std::optional<Response> activateViewerPass(ItemConstIterator item);
    std::optional<Response> wearSticker(ItemIterator item, std::uint8_t slot);
    std::optional<Response> addNameTag(ItemIterator item, ItemConstIterator nameTagItem, std::string_view nameTag);
    std::optional<Response> removeNameTag(ItemIterator item);
    std::optional<Response> openContainer(ItemConstIterator item, std::optional<ItemConstIterator> key);
    std::optional<Response> activateSouvenirToken(ItemConstIterator item, ItemIterator tournamentCoin);
    std::optional<Response> unsealGraffiti(ItemIterator item);
    std::optional<Response> swapStatTrak(ItemIterator itemFrom, ItemIterator itemTo, ItemConstIterator statTrakSwapTool);

private:
    BackendSimulator& backend;
    const game_items::Lookup& gameItemLookup;
};

}
