#pragma once
#include "CBaseClientState.hpp"
#include "IBaseClientDLL.hpp"
#include "ILocalize.hpp"
#include "IVEngineClient.hpp"

#define CLIENT_DLL_INTERFACE_VERSION		"VClient018"
extern sdk::IBaseClientDLL*					g_client;

#define ILOCALIZE_CLIENT_INTERFACE_VERSION	"Localize_001"
extern sdk::ILocalize*						g_localize;

extern sdk::CBaseClientState**				g_client_state;
