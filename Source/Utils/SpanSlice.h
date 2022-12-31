#pragma once

#include <algorithm>
#include <cstddef>
#include <span>

template <std::size_t Size, typename T>
class SpanSlice {
public:
    SpanSlice(std::span<T> span, std::size_t centerOffset) noexcept
        : span{ span }, centerOffset{ centerOffset }
    {
    }

    [[nodiscard]] std::span<T> operator()() const noexcept
    {
        return span.subspan(getSliceStartOffset(), getSliceSize());
    }

private:
    [[nodiscard]] constexpr std::size_t getSliceSize() const noexcept
    {
        return (std::min)(span.size(), Size);
    }

    [[nodiscard]] constexpr std::size_t getSliceStartOffset() const noexcept
    {
        if (centerOffset < getSliceSize() / 2)
            return 0;
        if (centerOffset > span.size() - getSliceSize() / 2)
            return span.size() - getSliceSize();
        return centerOffset - getSliceSize() / 2;
    }

    std::span<T> span;
    std::size_t centerOffset;
};
