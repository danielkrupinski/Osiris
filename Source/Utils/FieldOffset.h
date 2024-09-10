#pragma once

#include <concepts>

#include "Optional.h"

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

    [[nodiscard]] Optional<FieldType> toOptional() const noexcept
    {
        if (field)
            return *field;
        return {};
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

template <>
struct FieldValueProxy<void> {
    explicit FieldValueProxy(void* field) noexcept
        : field{field}
    {
    }

    [[nodiscard]] void* get() const noexcept
    {
        return field;
    }

private:
    void* field;
};

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

    [[nodiscard]] FieldValueProxy<FieldType> of(ClassType* thisptr) const noexcept
    {
        using BytePointer = std::byte*;
        using FieldPointer = FieldType*;
        if (thisptr != nullptr && offset != OffsetType{})
            return FieldValueProxy{ FieldPointer(BytePointer(thisptr) + offset) };
        return FieldValueProxy<FieldType>{ nullptr };
    }

private:
    OffsetType offset{};
};
