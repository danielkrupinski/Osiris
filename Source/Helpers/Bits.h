#pragma once

#include <cassert>
#include <concepts>

#include <Platform/Macros/IsPlatform.h>

#if IS_WIN64()
#include <intrin.h>
#endif

namespace bits
{

[[nodiscard]] inline int countrZero(unsigned int x) noexcept
{
    assert(x != 0 && "x must be non-zero");
#if IS_WIN64()
    unsigned long result;
    _BitScanForward(&result, x);
    return static_cast<int>(result);
#elif IS_LINUX()
    return __builtin_ctz(x);
#endif
}

template <std::unsigned_integral T>
[[nodiscard]] constexpr T clearRightmostSetBit(T value) noexcept
{
    return value & (value - 1);
}

}
