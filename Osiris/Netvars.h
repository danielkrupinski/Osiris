#pragma once

#include <string_view>
#include <unordered_map>

#include "nSkinz/Utilities/fnv_hash.hpp"

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

    auto get_offset(const fnv::hash hash) const noexcept
    {
        return props.at(hash).offset;
    }

    auto get_prop(const fnv::hash hash) const noexcept
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
    std::unordered_map<fnv::hash, recvData> props;
};

extern Netvars netvars;

#define PNETVAR_OFFSET(funcname, class_name, var_name, offset, ...) \
auto funcname() -> std::add_pointer_t<__VA_ARGS__> \
{ \
	constexpr auto hash = fnv::hash_constexpr(class_name "->" var_name); \
	const auto addr = std::uintptr_t(this) + offset + netvars.get_offset(hash); \
	return reinterpret_cast<std::add_pointer_t<__VA_ARGS__>>(addr); \
}

#define PNETVAR(funcname, class_name, var_name, ...) \
	PNETVAR_OFFSET(funcname, class_name, var_name, 0, __VA_ARGS__)

#define NETVAR_OFFSET(funcname, class_name, var_name, offset, ...) \
auto funcname() -> std::add_lvalue_reference_t<__VA_ARGS__> \
{ \
	constexpr auto hash = fnv::hash_constexpr(class_name "->" var_name); \
	const auto addr = std::uintptr_t(this) + offset + netvars.get_offset(hash); \
	return *reinterpret_cast<std::add_pointer_t<__VA_ARGS__>>(addr); \
}

#define NETVAR(funcname, class_name, var_name, ...) \
	NETVAR_OFFSET(funcname, class_name, var_name, 0, __VA_ARGS__)
