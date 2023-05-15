#pragma once

#include <cstdint>

#include "RetSpoofInvoker.h"

template <typename ReturnType, typename... Args>
class StdcallFunctionInvoker {
public:
    explicit StdcallFunctionInvoker(RetSpoofInvoker invoker, std::uintptr_t functionAddress)
        : invoker{ invoker }, functionAddress{ functionAddress }
    {
    }

    explicit StdcallFunctionInvoker(RetSpoofInvoker invoker, ReturnType(STDCALL_CONV* function)(Args...))
        : invoker{ invoker }, functionAddress{ std::uintptr_t(function) }
    {
    }

    ReturnType operator()(Args... args) const noexcept
    {
        return invoker.invokeStdcall<ReturnType, Args...>(functionAddress, args...);
    }

    [[nodiscard]] explicit operator bool() const noexcept
    {
        return functionAddress != 0;
    }

private:
    RetSpoofInvoker invoker;
    std::uintptr_t functionAddress;
};
