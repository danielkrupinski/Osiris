#pragma once

#include <cstdint>
#include <type_traits>

#include "fnv.h"

#include "SDK/Recv.h"

struct ProxyHook {
    recvProxy originalProxy = nullptr;
    recvProxy* addressOfOriginalProxy = nullptr;
};

struct ProxyHooks {
    ProxyHook spotted;
    ProxyHook viewModelSequence;
};

inline ProxyHooks proxyHooks;
class Client;

namespace Netvars
{
    void init(const Client& client) noexcept;
    void restore() noexcept;
    std::uint32_t get(std::uint32_t hash) noexcept;
}

#define PNETVAR_OFFSET(funcname, class_name, var_name, offset, type) \
[[nodiscard]] auto funcname() noexcept \
{ \
    constexpr auto hash = fnv::hash(class_name "->" var_name); \
    return reinterpret_cast<std::add_pointer_t<type>>(std::uintptr_t(this) + Netvars::get(hash) + offset); \
}

#define PNETVAR_OFFSET2(funcname, class_name, var_name, offset, type) \
[[nodiscard]] auto funcname() const noexcept \
{ \
    constexpr auto hash = fnv::hash(class_name "->" var_name); \
    return reinterpret_cast<std::add_pointer_t<type>>(getThis() + Netvars::get(hash) + offset); \
}

#define PNETVAR(funcname, class_name, var_name, type) \
    PNETVAR_OFFSET(funcname, class_name, var_name, 0, type)

#define PNETVAR2(funcname, class_name, var_name, type) \
    PNETVAR_OFFSET2(funcname, class_name, var_name, 0, type)

#define NETVAR_OFFSET(funcname, class_name, var_name, offset, type) \
[[nodiscard]] std::add_lvalue_reference_t<type> funcname() noexcept \
{ \
    constexpr auto hash = fnv::hash(class_name "->" var_name); \
    return *reinterpret_cast<std::add_pointer_t<type>>(std::uintptr_t(this) + Netvars::get(hash) + offset); \
}

#define NETVAR_OFFSET2(funcname, class_name, var_name, offset, type) \
[[nodiscard]] std::add_lvalue_reference_t<type> funcname() const noexcept \
{ \
    constexpr auto hash = fnv::hash(class_name "->" var_name); \
    return *reinterpret_cast<std::add_pointer_t<type>>(getThis() + Netvars::get(hash) + offset); \
}

#define NETVAR(funcname, class_name, var_name, type) \
    NETVAR_OFFSET(funcname, class_name, var_name, 0, type)

#define NETVAR2(funcname, class_name, var_name, type) \
    NETVAR_OFFSET2(funcname, class_name, var_name, 0, type)
