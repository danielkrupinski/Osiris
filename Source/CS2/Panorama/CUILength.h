#pragma once

namespace cs2
{

struct CUILength {
    enum EUILengthTypes {
        k_EUILengthUnset,
        k_EUILengthLength,
        k_EUILengthPercent
    };

    [[nodiscard]] static constexpr CUILength pixels(float value) noexcept
    {
        return CUILength{value, k_EUILengthLength};
    }

    [[nodiscard]] static constexpr CUILength percent(float value) noexcept
    {
        return CUILength{value, k_EUILengthPercent};
    }

    constexpr bool operator==(const CUILength&) const = default;

    float m_flValue;
    EUILengthTypes m_eType;
};

}
