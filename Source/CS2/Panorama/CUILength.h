#pragma once

namespace cs2
{

struct CUILength {
    enum EUILengthTypes {
        k_EUILengthUnset,
        k_EUILengthLength,
        k_EUILengthPercent,
        k_EUILengthFitChildren,
        k_EUILengthFillParentFlow,
        k_EUILengthHeightPercentage,
        k_EUILengthWidthPercentage
    };

    [[nodiscard]] static constexpr CUILength pixels(float value) noexcept
    {
        return CUILength{value, k_EUILengthLength};
    }

    [[nodiscard]] static constexpr CUILength percent(float value) noexcept
    {
        return CUILength{value, k_EUILengthPercent};
    }

    [[nodiscard]] static constexpr CUILength fillParentFlow(float value) noexcept
    {
        return CUILength{value, k_EUILengthFillParentFlow};
    }

    constexpr bool operator==(const CUILength&) const = default;

    float m_flValue;
    EUILengthTypes m_eType;
};

}
