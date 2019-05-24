#pragma once
#include "CBaseClientState.hpp"
#include "IBaseClientDLL.hpp"
#include "IClientEntityList.hpp"
#include "IGameEventManager2.hpp"
#include "ILocalize.hpp"
#include "IVEngineClient.hpp"
#include "IVModelInfoClient.hpp"

#define CLIENT_DLL_INTERFACE_VERSION		"VClient018"
extern sdk::IBaseClientDLL*					g_client;

#define VCLIENTENTITYLIST_INTERFACE_VERSION "VClientEntityList003"
extern sdk::IClientEntityList*				g_entity_list;

#define VENGINE_CLIENT_INTERFACE_VERSION	"VEngineClient014"
extern sdk::IVEngineClient*					g_engine;

#define VMODELINFO_CLIENT_INTERFACE_VERSION "VModelInfoClient004"
extern sdk::IVModelInfoClient*				g_model_info;

#define INTERFACEVERSION_GAMEEVENTSMANAGER2	"GAMEEVENTSMANAGER002"
extern sdk::IGameEventManager2*				g_game_event_manager;

#define ILOCALIZE_CLIENT_INTERFACE_VERSION	"Localize_001"
extern sdk::ILocalize*						g_localize;

#define INPUTSYSTEM_INTERFACE_VERSION		"InputSystemVersion001"
extern sdk::IInputSystem*					g_input_system;

extern sdk::CBaseClientState**				g_client_state;
extern sdk::C_CS_PlayerResource**			g_player_resource;