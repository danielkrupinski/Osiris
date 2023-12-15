#pragma once

#include <cassert>
#include <cstdint>

struct ToRvaConverter {
    [[nodiscard]] std::uint32_t toRva(std::uintptr_t address) const noexcept
    {
        assert(address >= baseAddress);
        return static_cast<std::uint32_t>(address - baseAddress);
    }

    std::uintptr_t baseAddress;
};
