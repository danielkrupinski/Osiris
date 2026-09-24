#pragma once

#include <cstddef>
#include <concepts>
#include <type_traits>

#include "OptionalPointee.h"

template <typename ClassType, typename TField, std::integral TOffset>
struct FieldOffset {
    using OffsetType = TOffset;
    using FieldType = TField;

    FieldOffset() = default;

    explicit FieldOffset(const OffsetType* offsetPtr) noexcept
        : offset{offsetPtr ? *offsetPtr : OffsetType{}}
    {
    }

    explicit FieldOffset(OffsetType offset) noexcept
        : offset{offset}
    {
    }

    [[nodiscard]] explicit operator bool() const noexcept
    {
        return offset > 0;
    }

    [[nodiscard]] OptionalPointee<FieldType> of(ClassType* thisptr) const noexcept
    {
        using BytePointer = std::byte*;
        using FieldPointer = FieldType*;
        if (thisptr != nullptr && offset > 0)
            return OptionalPointee{ FieldPointer(BytePointer(thisptr) + offset) };
        return {};
    }

    [[nodiscard]] OptionalPointee<std::remove_extent_t<FieldType>> arrayOf(ClassType* thisptr) const noexcept
        requires std::is_array_v<FieldType>
    {
        using BytePointer = std::byte*;
        using ElementType = std::remove_extent_t<FieldType>;
        using ElementPointer = ElementType*;
        if (thisptr != nullptr && offset > 0)
            return OptionalPointee{ ElementPointer(BytePointer(thisptr) + offset) };
        return {};
    }

private:
    OffsetType offset{};
};
