#pragma once

#include <cstddef>
#include <cstdint>
#include <memory>

#include <Platform/Macros/CallingConventions.h>
#include <Platform/Macros/PlatformSpecific.h>
#include <Vmt/VmtLengthCalculator.h>

class VmtSwap {
public:
    explicit VmtSwap(VmtLengthCalculator vmtLengthCalculator)
        : vmtLengthCalculator{ vmtLengthCalculator }
    {
    }

    void init(void* base) noexcept;
    void restore() noexcept
    {
        *reinterpret_cast<std::uintptr_t**>(base) = oldVmt;
    }

    template <typename T>
    std::uintptr_t hookAt(std::size_t index, T fun) const noexcept
    {
        newVmt[index + dynamicCastInfoLength] = reinterpret_cast<std::uintptr_t>(fun);
        return oldVmt[index];
    }

private:
    static constexpr auto dynamicCastInfoLength = WIN32_LINUX(1, 2);

    VmtLengthCalculator vmtLengthCalculator;
    void* base = nullptr;
    std::uintptr_t* oldVmt = nullptr;
    std::unique_ptr<std::uintptr_t[]> newVmt;
    std::size_t length = 0;
};
