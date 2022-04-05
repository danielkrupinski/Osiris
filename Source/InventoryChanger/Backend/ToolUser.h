#pragma once

#include <optional>

#include <InventoryChanger/Inventory/Item.h>
#include <InventoryChanger/StaticData.h>
#include <InventoryChanger/GameItems/Lookup.h>

#include "Response.h"

namespace inventory_changer::backend
{

class BackendSimulator;

class ToolUser {
public:
    std::optional<Response> applySticker(BackendSimulator& backend, std::list<inventory::Item_v2>::iterator item, std::list<inventory::Item_v2>::const_iterator sticker, std::uint8_t slot);
    void activateOperationPass(BackendSimulator& backend, std::list<inventory::Item_v2>::const_iterator item);
};

}
