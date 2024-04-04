#pragma once

#include <cassert>
#include <cstddef>
#include <cstdint>
#include <cstring>
#include <span>

#include <Utils/GenericPointer.h>

class PatternSearchResult {
public:
    PatternSearchResult(GenericPointer base, std::size_t patternFoundOffset, std::span<const std::byte> foundPatternBytes) noexcept
        : base{base}
        , patternFoundOffset{patternFoundOffset}
        , foundPatternBytes{foundPatternBytes}
    {
        assert(base);
    }

    PatternSearchResult() = default;

    PatternSearchResult& add(std::size_t offset) noexcept
    {
        if (base) {
            extraOffset += offset;
            assert(extraOffset < foundPatternBytes.size());
        }
        return *this;
    }

    [[nodiscard]] GenericPointer abs() const noexcept
    {
        if (base) {
            using OffsetType = std::int32_t;
            OffsetType offset;
            assert(foundPatternBytes.size() - extraOffset >= sizeof(OffsetType));
            std::memcpy(&offset, foundPatternBytes.data() + extraOffset, sizeof(OffsetType));
            return base.as<const std::byte*>() + patternFoundOffset + extraOffset + sizeof(OffsetType) + offset;
        }
        return {};
    }

    template <typename FieldOffsetType>
    [[nodiscard]] FieldOffsetType readOffset() const noexcept
    {
        if (base) {
            typename FieldOffsetType::OffsetType result;
            assert(foundPatternBytes.size() - extraOffset >= sizeof(result));
            std::memcpy(&result, foundPatternBytes.data() + extraOffset, sizeof(result));
            return FieldOffsetType{result};
        }
        return {};
    }

    template <typename T>
    [[nodiscard]] T as() const noexcept
    {
        if (base)
            return T(base.as<const std::byte*>() + patternFoundOffset + extraOffset);
        return T(base.as<void*>());
    }

private:
    GenericPointer base{};
    std::size_t patternFoundOffset;
    std::span<const std::byte> foundPatternBytes;
    std::size_t extraOffset{0};
};
