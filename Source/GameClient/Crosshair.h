#pragma once

#include <CS2/Classes/ConVarTypes.h>
#include <CS2/Constants/ColorConstants.h>
#include <CS2/Constants/CrosshairColorIndex.h>
#include <HookContext/HookContextMacros.h>
#include <Utils/Optional.h>

template <typename HookContext>
class Crosshair {
public:
    explicit Crosshair(HookContext& hookContext) noexcept
        : hookContext{hookContext}
    {
    }

    [[nodiscard]] Optional<cs2::Color> getColor() const
    {
        if (const auto colorIndex = GET_CONVAR_VALUE(cs2::cl_crosshaircolor); colorIndex.has_value())
            return cs2::CrosshairColorIndex{*colorIndex} == cs2::CrosshairColorIndex::RGB ? colorRGB() : colorFixed(*colorIndex);
        return {};
    }

private:
    [[nodiscard]] Optional<cs2::Color> colorRGB() const
    {
        const auto r = GET_CONVAR_VALUE(cs2::cl_crosshaircolor_r);
        const auto g = GET_CONVAR_VALUE(cs2::cl_crosshaircolor_g);
        const auto b = GET_CONVAR_VALUE(cs2::cl_crosshaircolor_b);
        if (r.has_value() && g.has_value() && b.has_value())
            return cs2::Color{static_cast<std::uint8_t>(*r), static_cast<std::uint8_t>(*g), static_cast<std::uint8_t>(*b)};
        return {};
    }

    [[nodiscard]] static constexpr cs2::Color colorFixed(cs2::cl_crosshaircolor::ValueType colorIndex) noexcept
    {
        switch (cs2::CrosshairColorIndex{colorIndex}) {
        using enum cs2::CrosshairColorIndex;
        case Red: return cs2::kColorCrosshairRed;
        case Green: return cs2::kColorCrosshairGreen;
        case Yellow: return cs2::kColorCrosshairYellow;
        case Blue: return cs2::kColorCrosshairBlue;
        case Cyan: return cs2::kColorCrosshairCyan;
        default: return cs2::kColorCrosshairGreen;
        }
    }

    HookContext& hookContext;
};
