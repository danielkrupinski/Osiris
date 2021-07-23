#pragma once

#include <cstddef>
#include <utility>

struct InventoryItem;

namespace ItemGenerator
{
    std::pair<std::size_t, std::size_t> generateItemFromContainer(const InventoryItem& caseItem) noexcept;
    std::size_t createDefaultDynamicData(std::size_t gameItemIndex) noexcept;
}
