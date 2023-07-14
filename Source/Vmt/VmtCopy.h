#pragma once

#include <cstdint>
#include <cstddef>
#include <memory>
#include <new>

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
            return replacementVmtWithTypeInfo.get() + platform::lengthOfTypeInfoPrecedingVmt;
        return nullptr;
    }

    [[nodiscard]] std::uintptr_t* getOriginalVmt() const noexcept
    {
        return originalVmt;
    }

private:
    [[nodiscard]] std::uintptr_t* allocateReplacementVmtWithTypeInfo() const noexcept
    {
        return new (std::nothrow) std::uintptr_t[lengthWithTypeInfo()];
    }

    void copyOriginalVmt() const noexcept
    {
        if (replacementVmtWithTypeInfo) [[likely]]
            std::copy_n(originalVmt - platform::lengthOfTypeInfoPrecedingVmt, lengthWithTypeInfo(), replacementVmtWithTypeInfo.get());
    }

    [[nodiscard]] std::size_t lengthWithTypeInfo() const noexcept
    {
        return length + platform::lengthOfTypeInfoPrecedingVmt;
    }

    std::uintptr_t* originalVmt;
    std::size_t length;
    std::unique_ptr<std::uintptr_t[]> replacementVmtWithTypeInfo;
};
