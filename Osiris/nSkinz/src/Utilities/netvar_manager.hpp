/* This file is part of nSkinz by namazso, licensed under the MIT license:
*
* MIT License
*
* Copyright (c) namazso 2018
*
* Permission is hereby granted, free of charge, to any person obtaining a copy
* of this software and associated documentation files (the "Software"), to deal
* in the Software without restriction, including without limitation the rights
* to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
* copies of the Software, and to permit persons to whom the Software is
* furnished to do so, subject to the following conditions:
*
* The above copyright notice and this permission notice shall be included in all
* copies or substantial portions of the Software.
*
* THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
* IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
* FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
* AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
* LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
* OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
* SOFTWARE.
*/
#pragma once
#include "../SDK/DataTable.hpp"
#include "fnv_hash.hpp"
#include <map>
#include <type_traits>

class netvar_manager
{
private:
	struct stored_data
	{
		sdk::RecvProp* prop_ptr;
		std::uint16_t class_relative_offset;
	};

public:
	// Return a const instance, others shouldnt modify this.
	static auto get() -> const netvar_manager&
	{
		static netvar_manager instance;
		return instance;
	}

	auto get_offset(const fnv::hash hash) const -> std::uint16_t
	{
		return m_props.at(hash).class_relative_offset;
	}

	auto get_prop(const fnv::hash hash) const -> sdk::RecvProp*
	{
		return m_props.at(hash).prop_ptr;
	}

	// Prevent instruction cache pollution caused by automatic
	// inlining of `get` and get_offset every netvar usage when
	// there are a lot of netvars
	__declspec(noinline) static auto get_offset_by_hash(const fnv::hash hash) -> std::uint16_t
	{
		return get().get_offset(hash);
	}
	
	template<fnv::hash Hash>
	static auto get_offset_by_hash_cached() -> std::uint16_t
	{
		static auto offset = std::uint16_t(0);
		if(!offset)
			offset = get_offset_by_hash(Hash);
		return offset;
	}

private:
	netvar_manager();
	auto dump_recursive(const char* base_class, sdk::RecvTable* table, std::uint16_t offset) -> void;

private:
	std::map<fnv::hash, stored_data> m_props;
};


#define PNETVAR_OFFSET(funcname, class_name, var_name, offset, ...) \
auto funcname() -> std::add_pointer_t<__VA_ARGS__> \
{ \
	constexpr auto hash = fnv::hash_constexpr(class_name "->" var_name); \
	const auto addr = std::uintptr_t(this) + offset + netvar_manager::get_offset_by_hash_cached<hash>(); \
	return reinterpret_cast<std::add_pointer_t<__VA_ARGS__>>(addr); \
}

#define PNETVAR(funcname, class_name, var_name, ...) \
	PNETVAR_OFFSET(funcname, class_name, var_name, 0, __VA_ARGS__)

#define NETVAR_OFFSET(funcname, class_name, var_name, offset, ...) \
auto funcname() -> std::add_lvalue_reference_t<__VA_ARGS__> \
{ \
	constexpr auto hash = fnv::hash_constexpr(class_name "->" var_name); \
	const auto addr = std::uintptr_t(this) + offset + netvar_manager::get_offset_by_hash_cached<hash>(); \
	return *reinterpret_cast<std::add_pointer_t<__VA_ARGS__>>(addr); \
}

#define NETVAR(funcname, class_name, var_name, ...) \
	NETVAR_OFFSET(funcname, class_name, var_name, 0, __VA_ARGS__)

#define NETPROP(funcname, class_name, var_name) \
static auto funcname() ->  RecvProp* \
{ \
	constexpr auto hash = fnv::hash_constexpr(class_name "->" var_name); \
	static sdk::RecvProp* prop_ptr; \
	if(!prop_ptr) prop_ptr = netvar_manager::get().get_prop(hash); \
	return prop_ptr; \
}