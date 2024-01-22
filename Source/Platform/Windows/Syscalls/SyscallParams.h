#pragma once

#include <cstdint>
#include <type_traits>

#include "WindowsSyscallIndex.h"

struct SyscallParams {
    WindowsSyscallIndex syscallIndex;
    std::uintptr_t firstParam;
};

static_assert(
    sizeof(std::declval<SyscallParams>().syscallIndex) == 8 &&
    sizeof(std::declval<SyscallParams>().firstParam) == 8 &&
    offsetof(SyscallParams, syscallIndex) == 0 &&
    offsetof(SyscallParams, firstParam) == 8,
    "Must match assembly");
