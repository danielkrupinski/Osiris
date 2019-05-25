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
#include "nSkinz.hpp"
#include "Hooks/hooks.hpp"
#include "kit_parser.hpp"
#include "config_.hpp"
#include "../../Interfaces.h"
#include "../../SDK/Engine.h"
#include "../../SDK/EntityList.h"

sdk::IClientEntityList*		g_entity_list;
sdk::ILocalize*				g_localize;

sdk::CBaseClientState**		g_client_state;
sdk::C_CS_PlayerResource**	g_player_resource;

void ensure_dynamic_hooks() noexcept
{
	if (const auto local = interfaces.entityList->getEntity(interfaces.engine->getLocalPlayer())) {
		static vmt_multi_hook player_hook;
		if (player_hook.initialize_and_hook_instance(local))
			player_hook.apply_hook<hooks::CCSPlayer_PostDataUpdate>(7);
	}
}

template <class T>
auto get_interface(const char* module, const char* name) -> T*
{
	return reinterpret_cast<T*>(platform::get_interface(module, name));
}

auto initializeNSkinz() -> void
{
	g_entity_list = get_interface<sdk::IClientEntityList>("client_panorama", VCLIENTENTITYLIST_INTERFACE_VERSION);
	g_localize = get_interface<sdk::ILocalize>("localize.dll", ILOCALIZE_CLIENT_INTERFACE_VERSION);
	g_client_state = *reinterpret_cast<sdk::CBaseClientState***>(get_vfunc<std::uintptr_t>(interfaces.engine, 12) + 0x10);

	game_data::initialize_kits();
}
