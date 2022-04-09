#pragma once

#include <optional>
#include <string_view>

#include <InventoryChanger/Inventory/Item.h>

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

    std::optional<Response> applySticker(std::list<inventory::Item_v2>::iterator item, std::list<inventory::Item_v2>::const_iterator sticker, std::uint8_t slot);
    std::optional<Response> applyPatch(std::list<inventory::Item_v2>::iterator item, std::list<inventory::Item_v2>::const_iterator patch, std::uint8_t slot);
    std::optional<Response> removePatch(std::list<inventory::Item_v2>::iterator item, std::uint8_t slot);
    void activateOperationPass(std::list<inventory::Item_v2>::const_iterator item);
    std::optional<Response> activateViewerPass(std::list<inventory::Item_v2>::const_iterator item);
    std::optional<Response> wearSticker(std::list<inventory::Item_v2>::iterator item, std::uint8_t slot);
    std::optional<Response> addNameTag(std::list<inventory::Item_v2>::iterator item, std::list<inventory::Item_v2>::const_iterator nameTagItem, std::string_view nameTag);
    std::optional<Response> removeNameTag(std::list<inventory::Item_v2>::iterator item);
    std::optional<Response> openContainer(std::list<inventory::Item_v2>::const_iterator item, std::optional<std::list<inventory::Item_v2>::const_iterator> key);
    std::optional<Response> activateSouvenirToken(std::list<inventory::Item_v2>::const_iterator item, std::list<inventory::Item_v2>::iterator tournamentCoin);
    std::optional<Response> unsealGraffiti(std::list<inventory::Item_v2>::iterator item);

private:
    BackendSimulator& backend;
    const game_items::Lookup& gameItemLookup;
};

}
