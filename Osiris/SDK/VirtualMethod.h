#pragma once

#include <cstddef>

namespace VirtualMethod
{
    template <typename T, std::size_t Idx, typename ...Args>
    constexpr auto call(void* classBase, Args... args) noexcept
    {
        return ((*reinterpret_cast<T(__thiscall***)(void*, Args...)>(classBase))[Idx])(classBase, args...);
    }
}

#define VIRTUAL_METHOD(returnType, name, idx, args, argsRaw) \
constexpr auto name##args noexcept \
{ \
    return VirtualMethod::call<returnType, idx>##argsRaw; \
}
