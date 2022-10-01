#pragma once

#include <cstddef>

#include "Platform.h"

#ifdef _WIN32
#include <x86RetSpoof.h>
#include "../RetSpoofGadgets.h"
#endif

#include <Platform/RetSpoofInvoker.h>

namespace VirtualMethod
{
    template <typename T, std::size_t Idx, typename... Args>
    constexpr T call(void* classBase, Args... args) noexcept
    {
#ifdef _WIN32
        return retSpoofGadgets.jmpEbxInClient.invokeThiscall<T, Args...>(std::uintptr_t(classBase), (*reinterpret_cast<std::uintptr_t**>(classBase))[Idx], args...);
#else
        return (*reinterpret_cast<T(THISCALL_CONV***)(void*, Args...)>(classBase))[Idx](classBase, args...);
#endif
    }
}

class VirtualCallable {
public:
    VirtualCallable(RetSpoofInvoker invoker, std::uintptr_t thisptr)
        : invoker{ invoker }, thisptr{ thisptr } {}

    template <typename ReturnType, std::size_t Idx, typename... Args>
    constexpr ReturnType call(Args... args) const noexcept
    {
        return invoker.invokeThiscall<ReturnType, Args...>(thisptr, (*reinterpret_cast<std::uintptr_t**>(thisptr))[Idx], args...);
    }

private:
    RetSpoofInvoker invoker;
    std::uintptr_t thisptr;
};

#define VIRTUAL_METHOD2(returnType, name, idx, args, argsRaw) \
returnType name args const noexcept \
{ \
    return call<returnType, idx>argsRaw; \
}

#define VIRTUAL_METHOD(returnType, name, idx, args, argsRaw) \
returnType name args noexcept \
{ \
    return VirtualMethod::call<returnType, idx>argsRaw; \
}

#ifdef _WIN32
#define VIRTUAL_METHOD_V(returnType, name, idx, args, argsRaw) VIRTUAL_METHOD(returnType, name, idx, args, argsRaw)
#else
#define VIRTUAL_METHOD_V(returnType, name, idx, args, argsRaw) VIRTUAL_METHOD(returnType, name, idx + 1, args, argsRaw)
#endif
