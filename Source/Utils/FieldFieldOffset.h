#pragma once

#include <concepts>
#include <cstddef>

#include "OptionalPointee.h"

template <typename ClassType, typename TField, std::integral TOffset, std::size_t offsetFromField>
class FieldFieldOffset {
public:
    using OffsetType = TOffset;
    using FieldType = TField;

    explicit FieldFieldOffset(OffsetType offset) noexcept
        : offset{offset}
    {
    }

    [[nodiscard]] OptionalPointee<FieldType> of(ClassType* thisptr) const noexcept
    {
        using BytePointer = std::byte*;
        using FieldPointer = FieldType*;
        if (thisptr != nullptr && offset != OffsetType{})
            return OptionalPointee{FieldPointer(BytePointer(thisptr) + offset - offsetFromField)};
        return {};
    }

private:
    OffsetType offset{};
};
