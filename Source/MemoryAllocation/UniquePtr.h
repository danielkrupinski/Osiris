#pragma once

#include <cstddef>
#include <memory>
#include <type_traits>

#include "MemoryAllocator.h"
#include "MemoryDeleter.h"

template <typename T>
using UniquePtr = std::unique_ptr<T, MemoryDeleter<T>>;

namespace mem
{

template <typename T>
    requires (!std::is_array_v<T>)
[[nodiscard]] auto makeUniqueForOverwrite() noexcept
{
    return UniquePtr<T>{ new (MemoryAllocator<T>::allocate()) T };
}

template <typename T>
    requires std::is_unbounded_array_v<T>
[[nodiscard]] auto makeUniqueForOverwrite(std::size_t size) noexcept
{
    return UniquePtr<T>{ new (MemoryAllocator<T>::allocate(size)) std::remove_extent_t<T>[size], MemoryDeleter<T>{ size } };
}

template <typename T, typename... Args>
    requires std::is_bounded_array_v<T>
void makeUniqueForOverwrite(Args&&...) = delete;

}
