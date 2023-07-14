#pragma once

#include <cstddef>
#include <span>

#include "VmtLength.h"
#include <Utils/MemorySection.h>

struct VmtLengthCalculator {
    explicit VmtLengthCalculator(MemorySection codeSection, MemorySection vmtSection)
        : codeSection{ codeSection }, vmtSection{ vmtSection }
    {
    }

    [[nodiscard]] VmtLength operator()(const std::uintptr_t* vmt) const noexcept
    {
        std::size_t length = 0;
        while (isVmtEntry(vmt + length))
            ++length;
        return VmtLength{ length };
    }

private:
    [[nodiscard]] bool isVmtEntry(const std::uintptr_t* pointer) const noexcept
    {
        return vmtSection.contains(std::uintptr_t(pointer), sizeof(std::uintptr_t)) && codeSection.contains(*pointer);
    }

    MemorySection codeSection;
    MemorySection vmtSection;
};
