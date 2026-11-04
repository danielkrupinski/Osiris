#pragma once

#include <cstdint>

struct LinuxMemory {
    [[nodiscard]] static bool isMemoryReadable(std::uintptr_t /* address */) noexcept
    {
        // todo: implement me
        return true;
    }
};
