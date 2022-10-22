#pragma once

#include "../JsonForward.h"
#include "../Interfaces.h"
#include "../Memory.h"

namespace inventory_changer
{
    class InventoryChanger;

    json toJson(const Interfaces& interfaces, const Memory& memory, const InventoryChanger& inventoryChanger);
    void fromJson(const json& j, InventoryChanger& inventoryChanger);
}
