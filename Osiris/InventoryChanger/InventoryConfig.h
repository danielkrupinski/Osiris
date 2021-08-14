#pragma once

#include "../JsonForward.h"

namespace InventoryChanger
{
    json toJson() noexcept;
    void fromJson(const json& j) noexcept;
    void resetConfig() noexcept;
}
