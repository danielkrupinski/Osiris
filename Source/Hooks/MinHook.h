#pragma once

#include <cstddef>
#include <memory>

#include <Platform/Macros/CallingConventions.h>
#include <Platform/Macros/IsPlatform.h>
#include <Utils/GenericFunctionPointer.h>
#include <Vmt/VmtLengthCalculator.h>

class MinHook {
public:
    explicit MinHook(VmtLengthCalculator)
    {
    }

    void install(std::uintptr_t*& vmt) noexcept;
    void uninstall(std::uintptr_t*&) noexcept {}

    [[nodiscard]] bool isInstalled(const std::uintptr_t*) const noexcept
    {
        return installed;
    }

    [[nodiscard]] GenericFunctionPointer hook(std::size_t index, GenericFunctionPointer fun) noexcept;

private:
    std::uintptr_t** vmt = nullptr;
    bool installed = false;
};
