#pragma once

#include <cstddef>
#include <memory>

#include <Platform/Macros/CallingConventions.h>
#include <Platform/Macros/IsPlatform.h>
#include <Vmt/VmtLengthCalculator.h>

class MinHook {
public:
    explicit MinHook(VmtLengthCalculator)
    {
    }

    void init(void* base) noexcept;
    void restore() noexcept {}
    std::uintptr_t hookAt(std::size_t index, void* fun) noexcept;

private:
    void* base;
};
