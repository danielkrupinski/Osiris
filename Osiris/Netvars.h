#pragma once

#include <cstdint>
#include <type_traits>

#include "fnv.h"

namespace Netvars
{
    void init() noexcept;
    void restore() noexcept;
    std::uint32_t get(std::uint32_t hash) noexcept;
}

#define PNETVAR_OFFSET(funcname, class_name, var_name, offset, type) \
[[nodiscard]] auto funcname() noexcept \
{ \
    constexpr auto hash = fnv::hash(class_name "->" var_name); \
    return reinterpret_cast<std::add_pointer_t<type>>(std::uintptr_t(this) + Netvars::get(hash) + offset); \
}

#define PNETVAR(funcname, class_name, var_name, type) \
    PNETVAR_OFFSET(funcname, class_name, var_name, 0, type)

#define NETVAR_OFFSET(funcname, class_name, var_name, offset, type) \
[[nodiscard]] std::add_lvalue_reference_t<type> funcname() noexcept \
{ \
    constexpr auto hash = fnv::hash(class_name "->" var_name); \
    return *reinterpret_cast<std::add_pointer_t<type>>(std::uintptr_t(this) + Netvars::get(hash) + offset); \
}

#define NETVAR(funcname, class_name, var_name, type) \
    NETVAR_OFFSET(funcname, class_name, var_name, 0, type)
