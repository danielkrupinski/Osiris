#pragma once

#include <Utils/MemorySection.h>

struct LinuxVmtFinderParams {
    MemorySection rodataSection;
    MemorySection dataRelRoSection;
};
