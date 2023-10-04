#pragma once

#include <cstdint>

namespace cs2
{

struct Color {
    Color(std::uint8_t r, std::uint8_t g, std::uint8_t b, std::uint8_t a = 255) noexcept
        : rgba{ r, g, b, a }
    {
    }

    [[nodiscard]] std::uint8_t r() const noexcept
    {
        return rgba[0];
    }

    [[nodiscard]] std::uint8_t g() const noexcept
    {
        return rgba[1];
    }

    [[nodiscard]] std::uint8_t b() const noexcept
    {
        return rgba[2];
    }

    [[nodiscard]] std::uint8_t a() const noexcept
    {
        return rgba[3];
    }

private:
    std::uint8_t rgba[4];
};

}
