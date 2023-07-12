#pragma once

#include <cstddef>
#include <cstdint>
#include <span>

struct MemorySection {
    MemorySection() = default;

    explicit MemorySection(std::span<const std::byte> section)
        : section{ section }
    {
    }

    [[nodiscard]] std::span<const std::byte> raw() const noexcept
    {
        return section;
    }

    [[nodiscard]] bool contains(std::uintptr_t address, std::size_t size) const noexcept
    {
        return address >= std::uintptr_t(section.data()) && section.size() >= size && (address - std::uintptr_t(section.data())) <= section.size() - size;
    }

    [[nodiscard]] bool contains(std::uintptr_t address) const noexcept
    {
        return address >= std::uintptr_t(section.data()) && address - std::uintptr_t(section.data()) < section.size();
    }

private:
    std::span<const std::byte> section{};
};
