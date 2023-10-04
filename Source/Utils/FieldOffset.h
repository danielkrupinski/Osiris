#pragma once

#include <concepts>

template <typename FieldType>
struct FieldValueProxy {
    explicit FieldValueProxy(FieldType* field) noexcept
        : field{ field }
    {
    }

    [[nodiscard]] FieldType* get() const noexcept
    {
        return field;
    }

    [[nodiscard]] FieldType valueOr(const FieldType& defaultValue) const noexcept
    {
        if (field)
            return *field;
        return defaultValue;
    }

    void operator=(const FieldType& value) const noexcept
    {
        if (field)
            *field = value;
    }

private:
    FieldType* field;
};

template <typename ClassType, typename FieldType, std::integral OffsetType>
struct FieldOffset {
    explicit FieldOffset(const OffsetType* offsetPtr) noexcept
        : offset{ offsetPtr ? *offsetPtr : OffsetType{} }
    {
    }

    [[nodiscard]] FieldValueProxy<FieldType> of(ClassType* thisptr) const noexcept
    {
        using BytePointer = std::byte*;
        using FieldPointer = FieldType*;
        if (offset != OffsetType{})
            return FieldValueProxy{ FieldPointer(BytePointer(thisptr) + offset) };
        return FieldValueProxy<FieldType>{ nullptr };
    }

private:
    OffsetType offset;
};
