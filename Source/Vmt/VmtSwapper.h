#pragma once

#include <cassert>
#include <cstddef>
#include <cstdint>
#include <optional>

#include "VmtCopy.h"
#include "VmtLengthCalculator.h"

#include <Utils/GenericFunctionPointer.h>

class VmtSwapper {
public:
    [[nodiscard]] bool isInstalled(const std::uintptr_t* vmt) const noexcept
    {
        return vmtCopy.has_value() && vmt == vmtCopy->getReplacementVmt();
    }

    bool install(const VmtLengthCalculator& vmtLengthCalculator, std::uintptr_t*& vmt) noexcept
    {
        const auto justInitialized = initializeVmtCopy(vmtLengthCalculator, vmt);
        vmt = vmtCopy->getReplacementVmt();
        return justInitialized;
    }

    void uninstall(std::uintptr_t*& vmt) const noexcept
    {
        assert(vmtCopy.has_value());
        vmt = vmtCopy->getOriginalVmt();
    }

    [[nodiscard]] GenericFunctionPointer hook(std::size_t index, GenericFunctionPointer replacementFunction) const noexcept
    {
        assert(vmtCopy.has_value());
        vmtCopy->getReplacementVmt()[index] = std::uintptr_t(static_cast<void(*)()>(replacementFunction));
        return reinterpret_cast<void(*)()>(vmtCopy->getOriginalVmt()[index]);
    }

private:
    [[nodiscard]] bool initializeVmtCopy(const VmtLengthCalculator& vmtLengthCalculator, std::uintptr_t* vmt) noexcept
    {
        if (!vmtCopy.has_value()) {
            vmtCopy.emplace(vmt, vmtLengthCalculator(vmt));
            return true;
        }
        return false;
    }

    std::optional<VmtCopy> vmtCopy;
};
