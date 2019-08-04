#pragma once

#include <string_view>
#include <unordered_map>

#include "fnv.h"

struct RecvProp;
struct RecvTable;

class Netvars final {
public:
    Netvars() noexcept;

    void restore() noexcept;

    auto get_offset(const uint32_t hash) const noexcept
    {
        return props.at(hash);
    }
private:
    void walkTable(bool, const char*, RecvTable*, const std::size_t = 0) noexcept;

    std::unordered_map<uint32_t, uint16_t> props;
};

extern Netvars netvars;

#define PNETVAR_OFFSET(funcname, class_name, var_name, offset, type) \
auto funcname() noexcept \
{ \
	return reinterpret_cast<std::add_pointer_t<type>>(this + netvars.get_offset(fnv::hash(class_name "->" var_name)) + offset); \
}

#define PNETVAR(funcname, class_name, var_name, type) \
	PNETVAR_OFFSET(funcname, class_name, var_name, 0, type)

#define NETVAR_OFFSET(funcname, class_name, var_name, offset, type) \
std::add_lvalue_reference_t<type> funcname() noexcept \
{ \
	return *reinterpret_cast<std::add_pointer_t<type>>(this + netvars.get_offset(fnv::hash(class_name "->" var_name)) + offset); \
}

#define NETVAR(funcname, class_name, var_name, type) \
	NETVAR_OFFSET(funcname, class_name, var_name, 0, type)
