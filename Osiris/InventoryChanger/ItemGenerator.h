#pragma once

#include <cstddef>
#include <utility>

namespace StaticData
{
    struct Case;
}

namespace ItemGenerator
{
    std::pair<std::size_t, std::size_t> generateItemFromContainer(const StaticData::Case& caseData) noexcept;
}
