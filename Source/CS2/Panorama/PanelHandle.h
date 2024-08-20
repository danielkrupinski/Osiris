#pragma once

#include <cstdint>

namespace cs2
{
    
struct PanelHandle {
    std::int32_t panelIndex = -1;
    std::uint32_t serialNumber;

    [[nodiscard]] bool isValid() const noexcept
    {
        return panelIndex != -1;
    }

    [[nodiscard]] constexpr bool operator==(const PanelHandle&) const = default;
};

}
