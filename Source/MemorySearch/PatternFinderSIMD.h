#pragma once

#include <cassert>
#include <span>
#include <string_view>

#include "PatternFinder.h"

class PatternFinderSIMD {
public:
    PatternFinderSIMD(std::span<const std::byte> bytes, std::string_view pattern);

    const std::byte* operator()() noexcept;

    [[nodiscard]] std::span<const std::byte> getNotCheckedBytes() const noexcept
    {
        return bytes.subspan(currentPos);
    }

private:
    std::size_t currentPos = 0;
    std::span<const std::byte> bytes;
    std::string_view pattern;
};
