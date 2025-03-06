#pragma once

#include <cstdint>
#include <limits>

namespace config_params
{
    using ObjectIndexType = std::uint16_t;
    using NestingLevelIndexType = std::uint8_t;

    static constexpr NestingLevelIndexType kMaxNestingLevel{7};
    static constexpr ObjectIndexType kInvalidObjectIndex{(std::numeric_limits<ObjectIndexType>::max)()};
    static constexpr ObjectIndexType kMaxObjectIndex{kInvalidObjectIndex - 1};
}
