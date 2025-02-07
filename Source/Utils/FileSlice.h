#pragma once

#include <cstddef>

struct FileSlice {
    [[nodiscard]] constexpr bool contains(FileSlice slice) const noexcept
    {
        return slice.startOffset >= startOffset && slice.length <= length && (slice.startOffset - startOffset) <= length - slice.length;
    }

    std::size_t startOffset{0};
    std::size_t length{0};
};
