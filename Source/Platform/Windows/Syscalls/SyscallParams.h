#pragma once

#include <cstddef>
#include <cstdint>

struct SyscallParams {
    std::uint64_t syscallIndex;
    std::uint64_t firstParam;
};

static_assert(
    offsetof(SyscallParams, syscallIndex) == 0 && offsetof(SyscallParams, firstParam) == 8,
    "Must match assembly");
