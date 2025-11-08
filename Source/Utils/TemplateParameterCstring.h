#pragma once

#include <array>
#include <cstddef>
#include <ranges>

template <std::size_t N>
struct TemplateParameterCstring {
    consteval TemplateParameterCstring(const char (&str)[N]) noexcept
    {
        std::ranges::copy(str, storage.begin());
    }

    [[nodiscard]] constexpr operator const char*() const noexcept
    {
        return storage.data();
    }

    std::array<char, N> storage;
};
