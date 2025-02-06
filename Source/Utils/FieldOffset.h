#pragma once

#include <cstddef>
#include <concepts>

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
        return offset != OffsetType{};
    }

    [[nodiscard]] OptionalPointee<FieldType> of(ClassType* thisptr) const noexcept
    {
        using BytePointer = std::byte*;
        using FieldPointer = FieldType*;
        if (thisptr != nullptr && offset != OffsetType{})
            return OptionalPointee{ FieldPointer(BytePointer(thisptr) + offset) };
        return {};
    }

private:
    OffsetType offset{};
};
