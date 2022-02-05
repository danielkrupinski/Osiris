#pragma once

#include <cstddef>

#include "Platform.h"

namespace VirtualMethod
{
    template <typename T, std::size_t Idx, typename ...Args>
    constexpr T call(void* classBase, Args... args) noexcept
    {
        return (*reinterpret_cast<T(THISCALL_CONV***)(void*, Args...)>(classBase))[Idx](classBase, args...);
    }
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
