#pragma once

#include <cstdint>

namespace cs2
{

struct Color {
    constexpr Color(std::uint8_t r, std::uint8_t g, std::uint8_t b, std::uint8_t a = 255) noexcept
        : rgba{ r, g, b, a }
    {
    }

    constexpr bool operator==(const Color&) const = default;

    [[nodiscard]] constexpr std::uint8_t r() const noexcept
    {
        return rgba[0];
    }

    [[nodiscard]] constexpr std::uint8_t g() const noexcept
    {
        return rgba[1];
    }

    [[nodiscard]] constexpr std::uint8_t b() const noexcept
    {
        return rgba[2];
    }

    [[nodiscard]] constexpr std::uint8_t a() const noexcept
    {
        return rgba[3];
    }

    [[nodiscard]] constexpr auto setAlpha(std::uint8_t newAlpha) const noexcept
    {
        return Color{r(), g(), b(), newAlpha};
    }

private:
    std::uint8_t rgba[4];
};

}
