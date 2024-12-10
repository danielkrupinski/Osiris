#pragma once

#include <cstdint>
#include <limits>

class InWorldPanelIndex {
public:
    using ValueType = std::uint16_t;

    static constexpr auto kNumberOfValueBits{std::numeric_limits<ValueType>::digits - 1};
    static constexpr ValueType kMaxValue{(1 << kNumberOfValueBits) - 1};
    static constexpr ValueType kMaxValidValue{kMaxValue - 1};

    [[nodiscard]] bool isValid() const noexcept
    {
        return value <= kMaxValidValue;
    }

    ValueType value{kMaxValue};
};
