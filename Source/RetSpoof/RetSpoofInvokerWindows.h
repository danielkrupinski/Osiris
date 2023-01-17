#pragma once

#include <cstdint>

#include <x86RetSpoof.h>

class RetSpoofInvokerWindows {
public:
    explicit RetSpoofInvokerWindows(std::uintptr_t gadgetAddress)
        : gadgetAddress{ gadgetAddress }
    {
    }

    template <typename ReturnType, typename... Args>
    ReturnType invokeFastcall(std::uintptr_t ecx, std::uintptr_t edx, std::uintptr_t functionAddress, Args... args) const noexcept
    {
        return x86RetSpoof::invokeFastcall<ReturnType, Args...>(ecx, edx, functionAddress, gadgetAddress, args...);
    }

    template <typename ReturnType, typename... Args>
    ReturnType invokeThiscall(std::uintptr_t ecx, std::uintptr_t functionAddress, Args... args) const noexcept
    {
        return x86RetSpoof::invokeThiscall<ReturnType, Args...>(ecx, functionAddress, gadgetAddress, args...);
    }

    template <typename ReturnType, typename... Args>
    ReturnType invokeStdcall(std::uintptr_t functionAddress, Args... args) const noexcept
    {
        return x86RetSpoof::invokeStdcall<ReturnType, Args...>(functionAddress, gadgetAddress, args...);
    }

    template <typename ReturnType, typename... Args>
    ReturnType invokeCdecl(std::uintptr_t functionAddress, Args... args) const noexcept
    {
        return x86RetSpoof::invokeCdecl<ReturnType, Args...>(functionAddress, gadgetAddress, args...);
    }

private:
    std::uintptr_t gadgetAddress;
};
