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
#include "netvar_manager.hpp"
#include "../SDK.hpp"
#include <cctype>
#include "../../Interfaces.h"
#include "../../SDK/Client.h"
#include "../../SDK/ClientClass.h"
#include "../../SDK/Recv.h"
NetvarManager netvarManager;

NetvarManager::NetvarManager()
{
	for (auto clazz = interfaces.client->getAllClasses(); clazz; clazz = clazz->next)
		if (clazz->recvTable)
			dump_recursive(clazz->networkName, clazz->recvTable, 0);
}

auto NetvarManager::dump_recursive(const char* base_class, RecvTable* table, const std::uint16_t offset) -> void
{
	for (auto i = 0; i < table->propCount; i++) {
		auto& prop_ptr = table->props[i];

		if (isdigit(prop_ptr.name[0]))
			continue;

		if (fnv::hash_runtime(prop_ptr.name) == FNV("baseclass"))
			continue;

		if (prop_ptr.type == 6 &&
			prop_ptr.dataTable &&
			prop_ptr.dataTable->netTableName[0] == 'D')
		{
			dump_recursive(base_class, prop_ptr.dataTable, std::uint16_t(offset + prop_ptr.offset));
		}

		char hash_name[256];

		strcpy_s(hash_name, base_class);
		strcat_s(hash_name, "->");
		strcat_s(hash_name, prop_ptr.name);

		const auto hash = fnv::hash_runtime(hash_name);
		const auto total_offset = std::uint16_t(offset + prop_ptr.offset);

		props[hash] =
		{
			&prop_ptr,
			total_offset
		};
	}
}