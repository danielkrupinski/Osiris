#pragma once

#include <optional>
#include <string_view>

#include <InventoryChanger/Inventory/Item.h>
#include <InventoryChanger/StaticData.h>
#include <InventoryChanger/GameItems/Lookup.h>

#include "Response.h"

namespace inventory_changer::backend
{

class BackendSimulator;

class ToolUser {
public:
    explicit ToolUser(BackendSimulator& backend) : backend{ backend } {}

    std::optional<Response> applySticker(std::list<inventory::Item_v2>::iterator item, std::list<inventory::Item_v2>::const_iterator sticker, std::uint8_t slot);
    std::optional<Response> applyPatch(std::list<inventory::Item_v2>::iterator item, std::list<inventory::Item_v2>::const_iterator patch, std::uint8_t slot);
    void activateOperationPass(std::list<inventory::Item_v2>::const_iterator item);
    std::optional<Response> activateViewerPass(std::list<inventory::Item_v2>::const_iterator item);
    std::optional<Response> wearSticker(std::list<inventory::Item_v2>::iterator item, std::uint8_t slot);
    std::optional<Response> addNameTag(std::list<inventory::Item_v2>::iterator item, std::list<inventory::Item_v2>::const_iterator nameTagItem, std::string_view nameTag);
    std::optional<Response> removeNameTag(std::list<inventory::Item_v2>::iterator item);
    std::optional<Response> openContainer(std::list<inventory::Item_v2>::const_iterator item, std::optional<std::list<inventory::Item_v2>::const_iterator> key);

private:
    BackendSimulator& backend;
};

}
