#pragma once

#include <cstddef>
#include <span>
#include <string_view>

class PatternFinderScalar {
public:
    PatternFinderScalar(std::span<const std::byte> bytes, std::string_view pattern);

    const std::byte* operator()() noexcept;

private:
    [[nodiscard]] std::size_t remainingBytes() const noexcept
    {
        return bytes.size() - currentPos;
    }

    std::size_t currentPos = 0;
    std::span<const std::byte> bytes;
    std::string_view pattern;
};

