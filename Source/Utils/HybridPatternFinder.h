#pragma once

#include <cstddef>
#include <span>
#include <string_view>

class HybridPatternFinder {
public:
    HybridPatternFinder(std::span<const std::byte> bytes, std::string_view pattern);

    [[nodiscard]] const std::byte* operator()() noexcept;

private:
    std::span<const std::byte> bytes;
    std::string_view pattern;
};
