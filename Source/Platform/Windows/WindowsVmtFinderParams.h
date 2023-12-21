#pragma once

#include <Utils/MemorySection.h>

struct WindowsVmtFinderParams {
    MemorySection dataSection;
    MemorySection rdataSection;
    const void* dllBase;
};
