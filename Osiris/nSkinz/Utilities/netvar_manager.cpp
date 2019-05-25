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

//#define DUMP_NETVARS

#ifdef DUMP_NETVARS
#define IF_DUMPING(...) __VA_ARGS__
#else
#define IF_DUMPING(...)
#endif

IF_DUMPING(static FILE* s_fp;)

netvar_manager::netvar_manager()
{
	IF_DUMPING(fopen_s(&s_fp, "netvar_dump.txt", "w");)
	for (auto clazz = interfaces.client->getAllClasses(); clazz; clazz = clazz->next)
		if (clazz->recvTable)
			dump_recursive(clazz->networkName, clazz->recvTable, 0);
	IF_DUMPING(fclose(s_fp);)
}

auto netvar_manager::dump_recursive(const char* base_class, RecvTable* table, const std::uint16_t offset) -> void
{
	for (auto i = 0; i < table->propCount; ++i)
	{
		const auto prop_ptr = &table->props[i];

		//Skip trash array items
		if (!prop_ptr || isdigit(prop_ptr->name[0]))
			continue;

		//We dont care about the base class, we already know that
		if (fnv::hash_runtime(prop_ptr->name) == FNV("baseclass"))
			continue;

		if (prop_ptr->type == sdk::DPT_DataTable &&
			prop_ptr->dataTable != nullptr &&
			prop_ptr->dataTable->netTableName[0] == 'D') // Skip shitty tables
		{
			dump_recursive(base_class, prop_ptr->dataTable, std::uint16_t(offset + prop_ptr->offset));
		}

		char hash_name[256];

		strcpy_s(hash_name, base_class);
		strcat_s(hash_name, "->");
		strcat_s(hash_name, prop_ptr->name);

		const auto hash = fnv::hash_runtime(hash_name);
		const auto total_offset = std::uint16_t(offset + prop_ptr->offset);

		IF_DUMPING(fprintf(s_fp, "%s\t0x%04X\t%s\n", base_class, total_offset, prop_ptr->m_pVarName);)

		m_props[hash] =
		{
			prop_ptr,
			total_offset
		};
        //NETPROP(GetTeamProp, "CPlayerResource", "m_iTeam");
        if (fnv::hash_runtime(base_class) == FNV("CPlayerResource") && fnv::hash_runtime(prop_ptr->name) == FNV("m_iTeam")) {
            const auto team_prop = prop_ptr->dataTable->props;
            const auto proxy_addr = std::uintptr_t(team_prop->proxy);
            //g_player_resource = *reinterpret_cast<sdk::C_CS_PlayerResource***>(proxy_addr + 0x10);
        }
	}
}