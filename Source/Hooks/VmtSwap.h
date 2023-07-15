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
#include <Vmt/VmtSwapper.h>

class VmtSwap {
public:
    explicit VmtSwap(const VmtLengthCalculator& vmtLengthCalculator)
        : swapper{ vmtLengthCalculator }
    {
    }

    void init(void* base) noexcept;
    void restore() noexcept
    {
        swapper.uninstall(*reinterpret_cast<std::uintptr_t**>(base));
    }

    template <typename T>
    std::uintptr_t hookAt(std::size_t index, T fun) const noexcept
    {
        return swapper.hook(index, std::uintptr_t(fun));
    }

private:
    void* base = nullptr;
    VmtSwapper swapper;
};
