#pragma once

#include <string_view>
#include <unordered_map>

#include "fnv.h"

struct RecvProp;
struct RecvTable;

class Netvars final {
public:
    Netvars() noexcept;

    auto operator[](const char* netvar) const noexcept
    {
        return offsets[netvar];
    }

    void restore() noexcept;

    auto get_offset(const uint32_t hash) const noexcept
    {
        return props.at(hash).offset;
    }

    auto get_prop(const uint32_t hash) const noexcept
    {
        return props.at(hash).prop;
    }
private:
    void loadTable(const char*, RecvTable*, const std::size_t = 0) noexcept;
    void unloadTable(RecvTable*) noexcept;
    mutable std::unordered_map<std::string_view, std::size_t> offsets;

    struct recvData {
        RecvProp* prop;
        uint16_t offset;
    };
    std::unordered_map<uint32_t, recvData> props;
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
