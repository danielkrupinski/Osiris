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
#include "../SDK.hpp"
#include "platform.hpp"
#include <vector>
#include <algorithm>

// Platform tools for windows. Maybe I'll make linux ones too

#include <Windows.h>
#include <psapi.h>

auto platform::get_export(const char* module_name, const char* export_name) -> void*
{
	HMODULE mod;
	while(!((mod = GetModuleHandleA(module_name))))
		Sleep(100);

	return reinterpret_cast<void*>(GetProcAddress(mod, export_name));
}

auto platform::get_interface(const char* module_name, const char* interface_name) -> void*
{
	const auto addr = get_export(module_name, "CreateInterface");
	const auto create_interface_fn = reinterpret_cast<sdk::CreateInterfaceFn>(addr);

	return create_interface_fn(interface_name, nullptr);
}

auto platform::get_module_info(const char* module_name) -> std::pair<std::uintptr_t, std::size_t>
{
	const auto module = GetModuleHandleA(module_name);
	if (!module)
		return { 0, 0 };
	MODULEINFO module_info;
	K32GetModuleInformation(GetCurrentProcess(), module, &module_info, sizeof(MODULEINFO));
	return { std::uintptr_t(module_info.lpBaseOfDll), module_info.SizeOfImage };
}

/*auto platform::find_pattern(const char* module_name, const char* pattern, const char* mask) -> std::uintptr_t
{
	MODULEINFO module_info = {};
	K32GetModuleInformation(GetCurrentProcess(), GetModuleHandleA(module_name), &module_info, sizeof(MODULEINFO));

	const auto address = reinterpret_cast<std::uint8_t*>(module_info.lpBaseOfDll);
	const auto size = module_info.SizeOfImage;

	std::vector<std::pair<std::uint8_t, bool>> signature;

	for (auto i = 0u; mask[i]; i++)
		signature.push_back(std::make_pair(pattern[i], mask[i] == 'x'));

	auto ret = std::search(address, address + size, signature.begin(), signature.end(),
		[](std::uint8_t curr, std::pair<std::uint8_t, bool> curr_pattern)
	{
		return (!curr_pattern.second) || curr == curr_pattern.first;
	});

	return ret == address + size ? 0 : std::uintptr_t(ret);
}*/

auto platform::is_code_ptr(void* ptr) -> bool
{
	constexpr const DWORD protect_flags = PAGE_EXECUTE | PAGE_EXECUTE_READ | PAGE_EXECUTE_READWRITE | PAGE_EXECUTE_WRITECOPY;

	MEMORY_BASIC_INFORMATION out;
	VirtualQuery(ptr, &out, sizeof out);

	return out.Type
		&& !(out.Protect & (PAGE_GUARD | PAGE_NOACCESS))
		&& out.Protect & protect_flags;
}
