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

//sdk::IBaseClientDLL*		g_client;
sdk::IClientEntityList*		g_entity_list;
sdk::IVEngineClient*		g_engine;
sdk::IVModelInfoClient*		g_model_info;
sdk::ILocalize*				g_localize;

sdk::CBaseClientState**		g_client_state;
sdk::C_CS_PlayerResource**	g_player_resource;

auto ensure_dynamic_hooks() -> void
{
	const auto local_index = g_engine->GetLocalPlayer();
	const auto local = static_cast<sdk::C_BasePlayer*>(g_entity_list->GetClientEntity(local_index));
	if(local)
	{
		static vmt_multi_hook player_hook;
		const auto networkable = static_cast<sdk::IClientNetworkable*>(local);
		if(player_hook.initialize_and_hook_instance(networkable))
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
	g_engine = get_interface<sdk::IVEngineClient>("engine.dll", VENGINE_CLIENT_INTERFACE_VERSION);
	g_model_info = get_interface<sdk::IVModelInfoClient>("engine.dll", VMODELINFO_CLIENT_INTERFACE_VERSION);
	g_localize = get_interface<sdk::ILocalize>("localize.dll", ILOCALIZE_CLIENT_INTERFACE_VERSION);

	g_client_state = *reinterpret_cast<sdk::CBaseClientState***>(get_vfunc<std::uintptr_t>(g_engine, 12) + 0x10);


	// Get skins
	game_data::initialize_kits();
}

// If we aren't unloaded correctly (like when you close csgo)
// we should just leak the hooks, since the hooked instances
// might be already destroyed
auto uninitialize() -> void
{
	//delete g_client_hook;
	//delete g_game_event_manager_hook;

	//delete g_sequence_hook;
}
