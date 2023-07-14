#pragma once

#include <cassert>
#include <cstddef>
#include <cstdint>
#include <memory>

#include <Platform/Macros/CallingConventions.h>
#include <Platform/Macros/PlatformSpecific.h>
#include <Platform/TypeInfoPrecedingVmt.h>
#include <Vmt/VmtCopy.h>
#include <Vmt/VmtLengthCalculator.h>

class VmtSwap {
public:
    explicit VmtSwap(const VmtLengthCalculator& vmtLengthCalculator)
        : vmtLengthCalculator{ vmtLengthCalculator }
    {
    }

    void init(void* base) noexcept;
    void restore() noexcept
    {
        assert(vmtCopy.has_value());
        *reinterpret_cast<std::uintptr_t**>(base) = vmtCopy->getOriginalVmt();
    }

    template <typename T>
    std::uintptr_t hookAt(std::size_t index, T fun) const noexcept
    {
        assert(vmtCopy.has_value());
        vmtCopy->getReplacementVmt()[index] = reinterpret_cast<std::uintptr_t>(fun);
        return vmtCopy->getOriginalVmt()[index];
    }

private:
    VmtLengthCalculator vmtLengthCalculator;
    void* base = nullptr;
    std::optional<VmtCopy> vmtCopy;
};
