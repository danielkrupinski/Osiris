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

#include <cassert>
#include <cstring>

#include "Platform.hpp"

class table_hook
{
public:
	constexpr table_hook()
		: m_new_vmt{nullptr}
		, m_old_vmt{nullptr} {}

	~table_hook()
	{
		if(m_new_vmt)
			delete[] (m_new_vmt - 1);
	}

protected:
	auto initialize(void** original_table) -> void
	{
		m_old_vmt = original_table;

		size_t size = 0;
		while(m_old_vmt[size] && platform::is_code_ptr(m_old_vmt[size]))
			++size;

		m_new_vmt = (new void*[size + 1]) + 1;
		//std::copy(m_old_vmt - 1, m_old_vmt + size, m_new_vmt - 1);
		memcpy(m_new_vmt - 1, m_old_vmt - 1, sizeof(void*) * (size + 1));
	}

	constexpr auto leak_table() -> void
	{
		m_new_vmt = nullptr;
	}

	auto hook_instance(void* inst) const -> void
	{
		auto& vtbl = *reinterpret_cast<void***>(inst);
		assert(vtbl == m_old_vmt || vtbl == m_new_vmt);
		vtbl = m_new_vmt;
	}

	auto unhook_instance(void* inst) const -> void
	{
		auto& vtbl = *reinterpret_cast<void***>(inst);
		assert(vtbl == m_old_vmt || vtbl == m_new_vmt);
		vtbl = m_old_vmt;
	}

	auto initialize_and_hook_instance(void* inst) -> bool
	{
		auto& vtbl = *reinterpret_cast<void***>(inst);
		auto initialized = false;
		if(!m_old_vmt)
		{
			initialized = true;
			initialize(vtbl);
		}
		hook_instance(inst);
		return initialized;
	}

	template <typename Fn>
	auto hook_function(Fn hooked_fn, const std::size_t index) -> Fn
	{
		m_new_vmt[index] = (void*)(hooked_fn);
		return (Fn)(m_old_vmt[index]);
	}

	template<typename T>
	auto apply_hook(std::size_t idx) -> void
	{
		T::m_original = hook_function(&T::hooked, idx);
	}

	template <typename Fn = uintptr_t>
	auto get_original_function(const int index) -> Fn
	{
		return (Fn)(m_old_vmt[index]);
	}

private:
	void** m_new_vmt = nullptr;
	void** m_old_vmt = nullptr;
};

class vmt_smart_hook : table_hook
{
public:
	vmt_smart_hook(void* class_base)
		: m_class{class_base}
	{
		initialize_and_hook_instance(class_base);
	}

	~vmt_smart_hook()
	{
		unhook_instance(m_class);
	}

	auto rehook() const -> void
	{
		hook_instance(m_class);
	}

	auto unhook() const -> void
	{
		unhook_instance(m_class);
	}

	using table_hook::apply_hook;
	using table_hook::get_original_function;
	using table_hook::hook_function;

private:
	void* m_class = nullptr;
};

class vmt_multi_hook : table_hook
{
public:
	constexpr vmt_multi_hook() {}

	~vmt_multi_hook()
	{
		leak_table();
	}

	using table_hook::apply_hook;
	using table_hook::get_original_function;
	using table_hook::hook_function;
	using table_hook::hook_instance;
	using table_hook::unhook_instance;
	using table_hook::initialize;
	using table_hook::initialize_and_hook_instance;
};