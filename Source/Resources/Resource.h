#pragma once

#include <array>
#include <cstddef>

namespace Resource
{
    template <std::size_t N>
    constexpr auto decodeBase85(const char(&input)[N]) noexcept
    {
        std::array<char, N * 4 / 5> out{};

        constexpr auto decode85Byte = [](char c) constexpr -> unsigned int { return c >= '\\' ? c - 36 : c - 35; };

        for (std::size_t i = 0, j = 0; i < N - 1; i += 5, j += 4) {
            unsigned int tmp = decode85Byte(input[i]) + 85 * (decode85Byte(input[i + 1]) + 85 * (decode85Byte(input[i + 2]) + 85 * (decode85Byte(input[i + 3]) + 85 * decode85Byte(input[i + 4]))));
            out[j] = ((tmp >> 0) & 0xFF); out[j + 1] = ((tmp >> 8) & 0xFF); out[j + 2] = ((tmp >> 16) & 0xFF); out[j + 3] = ((tmp >> 24) & 0xFF);
        }

        return out;
    }
}
