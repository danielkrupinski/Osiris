#pragma once

#include <cstddef>
#include <span>

#include "VmtLength.h"
#include <Utils/MemorySection.h>

struct VmtLengthCalculator {
    explicit VmtLengthCalculator(MemorySection codeSection)
        : codeSection{ codeSection }
    {
    }

    [[nodiscard]] VmtLength operator()(const std::uintptr_t* vmt) const noexcept
    {
        std::size_t length = 0;
        while (codeSection.contains(vmt[length]))
            ++length;
        return VmtLength{ length };
    }

private:
    MemorySection codeSection;
};
