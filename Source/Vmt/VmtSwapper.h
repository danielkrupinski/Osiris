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
    [[nodiscard]] bool wasEverInstalled() const noexcept
    {
        return vmtCopy.has_value();
    }

    [[nodiscard]] bool isInstalled(const std::uintptr_t* vmt) const noexcept
    {
        assert(wasEverInstalled());
        return vmt == vmtCopy->getReplacementVmt();
    }

    bool install(const VmtLengthCalculator& vmtLengthCalculator, std::uintptr_t*& vmt) noexcept
    {
        const auto justInitialized = initializeVmtCopy(vmtLengthCalculator, vmt);
        if (const auto replacementVmt = vmtCopy->getReplacementVmt())
            vmt = replacementVmt;
        return justInitialized;
    }

    void uninstall(std::uintptr_t*& vmt) const noexcept
    {
        assert(wasEverInstalled());
        vmt = vmtCopy->getOriginalVmt();
    }

    [[nodiscard]] GenericFunctionPointer hook(std::size_t index, GenericFunctionPointer replacementFunction) const noexcept
    {
        assert(wasEverInstalled());
        if (const auto replacementVmt = vmtCopy->getReplacementVmt())
            replacementVmt[index] = std::uintptr_t(static_cast<void(*)()>(replacementFunction));
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
