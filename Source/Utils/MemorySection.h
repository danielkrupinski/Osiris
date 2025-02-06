#pragma once

#include <cassert>
#include <cstddef>
#include <cstdint>
#include <span>

struct MemorySection {
    MemorySection() = default;

    explicit MemorySection(std::span<const std::byte> section) noexcept
        : base{reinterpret_cast<std::uintptr_t>(section.data())}
        , size{section.size()}
    {
    }

    [[nodiscard]] std::span<const std::byte> raw() const noexcept
    {
        return {reinterpret_cast<const std::byte*>(base), size};
    }

    [[nodiscard]] bool contains(std::uintptr_t address, std::size_t objectSize) const noexcept
    {
        return address >= base && size >= objectSize && (address - base) <= size - objectSize;
    }

    [[nodiscard]] bool contains(std::uintptr_t address) const noexcept
    {
        return address >= base && address - base < size;
    }

    [[nodiscard]] std::size_t offsetOf(std::uintptr_t address) const noexcept
    {
        assert(contains(address));
        return address - base;
    }

private:
    std::uintptr_t base{0};
    std::size_t size{0};
};
