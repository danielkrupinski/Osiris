#pragma once

#include <cstddef>
#include <cstring>
#include <type_traits>

namespace utils
{

template <typename Source, typename Dest>
    requires (sizeof(Source) == sizeof(Dest)
        && std::is_trivially_copyable_v<Source>
        && std::is_trivially_copyable_v<Dest>)
void copyBytes(const Source& source, Dest& dest) noexcept
{
    std::memcpy(reinterpret_cast<std::byte*>(&dest), &source, sizeof(Source));
}

template <typename T>
[[nodiscard]] T fromBytes(const auto& bytes) noexcept
{
    T result;
    copyBytes(bytes, result);
    return result;
}

}
