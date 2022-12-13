#pragma once

#include <cstdint>

#include <Platform/RetSpoofInvoker.h>

template <typename ReturnType, typename... Args>
class StdcallFunctionInvoker {
public:
    explicit StdcallFunctionInvoker(RetSpoofInvoker invoker, std::uintptr_t functionAddress)
        : invoker{ invoker }, functionAddress{ functionAddress }
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
