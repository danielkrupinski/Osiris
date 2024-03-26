#pragma once

namespace cs2
{

struct CEntityIndex {
    [[nodiscard]] constexpr bool isValid() const noexcept;

    int value;
};

constexpr CEntityIndex kMaxValidEntityIndex{0x7FFE};

constexpr bool CEntityIndex::isValid() const noexcept
{
    return value >= 0 && value <= kMaxValidEntityIndex.value;
}

}
