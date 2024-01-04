#pragma once

namespace cs2
{

struct CEntityIndex {
    [[nodiscard]] static consteval CEntityIndex maxValid() noexcept
    {
        return CEntityIndex{0x7FFE};
    }

    int value;
};

}
