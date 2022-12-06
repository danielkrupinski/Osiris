#pragma once

#include "../JsonForward.h"
#include "../Memory.h"

namespace inventory_changer
{
    class InventoryChanger;

    json toJson(const OtherInterfaces& interfaces, const Memory& memory, const InventoryChanger& inventoryChanger);
    void fromJson(const json& j, InventoryChanger& inventoryChanger);
}
