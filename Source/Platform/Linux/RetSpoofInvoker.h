#pragma once

#include <cstdint>

namespace linux_platform
{

class RetSpoofInvoker {
public:
    template <typename ReturnType, typename... Args>
    ReturnType invokeThiscall(std::uintptr_t thisptr, std::uintptr_t functionAddress, Args... args) const noexcept
    {
        return reinterpret_cast<ReturnType(*)(std::uintptr_t, Args...)>(functionAddress)(thisptr, args...);
    }

    template <typename ReturnType, typename... Args>
    ReturnType invokeCdecl(std::uintptr_t functionAddress, Args... args) const noexcept
    {
        return reinterpret_cast<ReturnType(*)(Args...)>(functionAddress)(args...);
    }
};

}
