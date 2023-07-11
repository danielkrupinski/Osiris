#pragma once

#include <cstddef>
#include <span>

#include "VmtLength.h"

struct VmtLengthCalculator {
    explicit VmtLengthCalculator(std::span<const std::byte> codeSection)
        : codeSection{ codeSection }
    {
    }

    [[nodiscard]] VmtLength operator()(const std::uintptr_t* vmt) const noexcept
    {
        std::size_t length = 0;
        while (vmt[length] >= std::uintptr_t(codeSection.data()) && vmt[length] < std::uintptr_t(codeSection.data() + codeSection.size()))
            ++length;
        return VmtLength{ length };
    }

private:
    std::span<const std::byte> codeSection;
};
