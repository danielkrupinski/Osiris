#pragma once

#include <bit>
#include <cstddef>
#include <cstdint>

#include <CS2/Classes/Vector.h>

namespace engine_trace {

constexpr std::size_t kDescriptorCapacity{0x30};
constexpr std::size_t kFilterCapacity{72};
constexpr std::size_t kOutputCapacity{0xC0};

struct Bounds6f {
    cs2::Vector mins;
    cs2::Vector maxs;
};

struct alignas(8) DescriptorStorage {
    std::byte storage[kDescriptorCapacity]{};
};

struct alignas(8) FilterStorage {
    std::byte storage[kFilterCapacity]{};
};

struct alignas(16) OutputStorage {
    std::byte storage[kOutputCapacity]{};
};

static_assert(sizeof(Bounds6f) == 24);
static_assert(offsetof(Bounds6f, mins) == 0);
static_assert(offsetof(Bounds6f, maxs) == sizeof(cs2::Vector));
static_assert(sizeof(DescriptorStorage) == kDescriptorCapacity);
static_assert(alignof(DescriptorStorage) == 8);
static_assert(sizeof(FilterStorage) == kFilterCapacity);
static_assert(alignof(FilterStorage) == 8);
static_assert(sizeof(OutputStorage) == kOutputCapacity);
static_assert(alignof(OutputStorage) == 16);

[[nodiscard]] constexpr bool isValidOutputOffset(std::int32_t offset, std::size_t fieldSize) noexcept
{
    return offset > 0 && static_cast<std::size_t>(offset) % alignof(float) == 0
        && static_cast<std::size_t>(offset) <= kOutputCapacity
        && fieldSize <= kOutputCapacity - static_cast<std::size_t>(offset);
}

[[nodiscard]] constexpr bool doRegionsOverlap(std::size_t firstOffset, std::size_t firstSize, std::size_t secondOffset, std::size_t secondSize) noexcept
{
    return firstOffset < secondOffset + secondSize && secondOffset < firstOffset + firstSize;
}

[[nodiscard]] constexpr bool areValidOutputOffsets(std::int32_t endPositionOffset, std::int32_t normalOffset, std::int32_t fractionOffset) noexcept
{
    return isValidOutputOffset(endPositionOffset, sizeof(cs2::Vector))
        && isValidOutputOffset(normalOffset, sizeof(cs2::Vector))
        && isValidOutputOffset(fractionOffset, sizeof(float))
        && !doRegionsOverlap(static_cast<std::size_t>(endPositionOffset), sizeof(cs2::Vector), static_cast<std::size_t>(normalOffset), sizeof(cs2::Vector))
        && !doRegionsOverlap(static_cast<std::size_t>(endPositionOffset), sizeof(cs2::Vector), static_cast<std::size_t>(fractionOffset), sizeof(float))
        && !doRegionsOverlap(static_cast<std::size_t>(normalOffset), sizeof(cs2::Vector), static_cast<std::size_t>(fractionOffset), sizeof(float));
}

[[nodiscard]] constexpr bool isFinite(float value) noexcept
{
    return (std::bit_cast<std::uint32_t>(value) & 0x7F800000u) != 0x7F800000u;
}

[[nodiscard]] constexpr bool isFinite(cs2::Vector value) noexcept
{
    return isFinite(value.x) && isFinite(value.y) && isFinite(value.z);
}

[[nodiscard]] constexpr bool hasUsableNormal(cs2::Vector normal) noexcept
{
    return isFinite(normal) && (normal.x != 0.0f || normal.y != 0.0f || normal.z != 0.0f);
}

}
