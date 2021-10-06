#pragma once

#include <cstddef>
#include <utility>

#include "StaticData.h"

struct InventoryItem;

namespace ItemGenerator
{
    std::pair<StaticData::ItemIndex, std::size_t> generateItemFromContainer(const InventoryItem& caseItem) noexcept;
    std::size_t createDefaultDynamicData(StaticData::ItemIndex gameItemIndex) noexcept;
}
