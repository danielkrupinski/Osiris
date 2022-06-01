#pragma once

#include "../JsonForward.h"

namespace inventory_changer
{
    class InventoryChanger;
}

namespace InventoryChanger
{
    void fromJson(const json& j) noexcept;
}

json toJson(const inventory_changer::InventoryChanger& inventoryChanger);
