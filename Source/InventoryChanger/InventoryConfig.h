#pragma once

#include "../JsonForward.h"

namespace inventory_changer
{
    class InventoryChanger;
}

json toJson(const inventory_changer::InventoryChanger& inventoryChanger);
void fromJson(const json& j, inventory_changer::InventoryChanger& inventoryChanger);
