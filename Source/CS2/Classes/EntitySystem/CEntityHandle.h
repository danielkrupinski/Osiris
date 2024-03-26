#pragma once

#include <cstdint>

#include "CEntityIndex.h"

namespace cs2
{

struct CEntityHandle {
    [[nodiscard]] CEntityIndex index() const noexcept
    {
        return CEntityIndex{static_cast<int>(value & 0x7FFF)};
    }

    [[nodiscard]] constexpr bool operator==(const CEntityHandle&) const = default;

    std::uint32_t value;
};

}
