#pragma once

#include "../JsonForward.h"

namespace inventory_changer
{
    class InventoryChanger;

    json toJson(const InventoryChanger& inventoryChanger);
    void fromJson(const json& j, InventoryChanger& inventoryChanger);
}
