#pragma once

#include <cstdint>

#include <Platform/Macros/CallingConventions.h>
#include "RetSpoofInvoker.h"

template <typename ReturnType, typename... Args>
class CdeclFunctionInvoker {
public:
    explicit CdeclFunctionInvoker(RetSpoofInvoker invoker, std::uintptr_t functionAddress)
        : invoker{ invoker }, functionAddress{ functionAddress }
    {
    }

    explicit CdeclFunctionInvoker(RetSpoofInvoker invoker, ReturnType(CDECL_CONV* function)(Args...))
        : invoker{ invoker }, functionAddress{ std::uintptr_t(function) }
    {
    }

    ReturnType operator()(Args... args) const noexcept
    {
        return invoker.invokeCdecl<ReturnType, Args...>(functionAddress, args...);
    }

    [[nodiscard]] explicit operator bool() const noexcept
    {
        return functionAddress != 0;
    }

private:
    RetSpoofInvoker invoker;
    std::uintptr_t functionAddress;
};
