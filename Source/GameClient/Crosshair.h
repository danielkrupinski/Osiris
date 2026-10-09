#pragma once

#include <CS2/Classes/ConVarTypes.h>
#include <CS2/Classes/Color.h>
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
        const auto r = GET_CONVAR_VALUE(cs2::cl_crosshaircolor_r);
        const auto g = GET_CONVAR_VALUE(cs2::cl_crosshaircolor_g);
        const auto b = GET_CONVAR_VALUE(cs2::cl_crosshaircolor_b);
        if (r.has_value() && g.has_value() && b.has_value())
            return cs2::Color{static_cast<std::uint8_t>(*r), static_cast<std::uint8_t>(*g), static_cast<std::uint8_t>(*b)};
        return {};
    }

private:
    HookContext& hookContext;
};
