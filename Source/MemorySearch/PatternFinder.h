#pragma once

#include <span>
#include <string_view>

struct PatternFinder {
public:
    explicit PatternFinder(std::span<const std::byte> bytes)
        : bytes{ bytes }
    {
    }

    [[nodiscard]] const std::byte* operator()(std::string_view pattern) const noexcept;

private:
    std::span<const std::byte> bytes;
};

[[nodiscard]] bool matchPattern(std::span<const std::byte> bytes, std::string_view pattern) noexcept;

namespace utils { constexpr auto wildcardChar = '?'; }
