#pragma once

#include <cstddef>
#include <span>
#include <string_view>

class HybridPatternFinder {
public:
    HybridPatternFinder(std::span<const std::byte> bytes, std::string_view pattern);

    [[nodiscard]] const std::byte* findSIMD();
    [[nodiscard]] const std::byte* findScalar();

private:
    [[nodiscard]] std::size_t remainingBytes() const noexcept
    {
        return bytes.size() - currentPos;
    }

    std::size_t currentPos = 0;
    std::span<const std::byte> bytes;
    std::string_view pattern;
};
