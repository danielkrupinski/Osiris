#pragma once

#include <cstdint>
#include <Platform/Macros/PlatformSpecific.h>

namespace cs2
{

struct LINUX_ONLY([[gnu::packed]]) CStyleSymbol {
    static constexpr auto kInvalidId{0xFF};

    [[nodiscard]] bool isValid() const noexcept
    {
        return m_Id != kInvalidId;
    }

    std::uint8_t m_Id{kInvalidId};
};

}
