#pragma once

#include <cstdint>

class RetSpoofInvokerLinux {
public:
    template <typename ReturnType, typename... Args>
    ReturnType invokeThiscall(std::uintptr_t thisptr, std::uintptr_t functionAddress, Args... args) const noexcept
    {
        return reinterpret_cast<ReturnType(*)(std::uintptr_t, Args...)>(functionAddress)(thisptr, args...);
    }

    template <typename ReturnType, typename... Args>
    ReturnType invokeFastcall(std::uintptr_t ecx, std::uintptr_t edx, std::uintptr_t functionAddress, Args... args) const noexcept
    {
        return reinterpret_cast<ReturnType(*)(std::uintptr_t, std::uintptr_t, Args...)>(functionAddress)(ecx, edx, args...);
    }

    template <typename ReturnType, typename... Args>
    ReturnType invokeStdcall(std::uintptr_t functionAddress, Args... args) const noexcept
    {
        return reinterpret_cast<ReturnType(*)(Args...)>(functionAddress)(args...);
    }

    template <typename ReturnType, typename... Args>
    ReturnType invokeCdecl(std::uintptr_t functionAddress, Args... args) const noexcept
    {
        return reinterpret_cast<ReturnType(*)(Args...)>(functionAddress)(args...);
    }
};
