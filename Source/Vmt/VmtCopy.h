#pragma once

#include <algorithm>
#include <cstdint>
#include <cstddef>

#include <MemoryAllocation/MemoryAllocator.h>
#include <Platform/TypeInfoPrecedingVmt.h>
#include "VmtLength.h"

class VmtCopy {
public:
    VmtCopy(std::uintptr_t* vmt, VmtLength length) noexcept
        : originalVmt{ vmt }
        , length{ static_cast<std::size_t>(length) }
        , replacementVmtWithTypeInfo{ allocateReplacementVmtWithTypeInfo() }
    {
        copyOriginalVmt();
    }

    [[nodiscard]] std::uintptr_t* getReplacementVmt() const noexcept
    {
        if (replacementVmtWithTypeInfo) [[likely]]
            return replacementVmtWithTypeInfo + platform::lengthOfTypeInfoPrecedingVmt;
        return nullptr;
    }

    [[nodiscard]] std::uintptr_t* getOriginalVmt() const noexcept
    {
        return originalVmt;
    }

    ~VmtCopy() noexcept
    {
        if (replacementVmtWithTypeInfo)
            MemoryAllocator::deallocate(reinterpret_cast<std::byte*>(replacementVmtWithTypeInfo), sizeof(std::uintptr_t) * lengthWithTypeInfo());
    }

private:
    [[nodiscard]] std::uintptr_t* allocateReplacementVmtWithTypeInfo() const noexcept
    {
        return new (MemoryAllocator::allocate(sizeof(std::uintptr_t) * lengthWithTypeInfo())) std::uintptr_t[lengthWithTypeInfo()];
    }

    void copyOriginalVmt() const noexcept
    {
        if (replacementVmtWithTypeInfo) [[likely]]
            std::copy_n(originalVmt - platform::lengthOfTypeInfoPrecedingVmt, lengthWithTypeInfo(), replacementVmtWithTypeInfo);
    }

    [[nodiscard]] std::size_t lengthWithTypeInfo() const noexcept
    {
        return length + platform::lengthOfTypeInfoPrecedingVmt;
    }

    std::uintptr_t* originalVmt;
    std::size_t length;
    std::uintptr_t* replacementVmtWithTypeInfo;
};
