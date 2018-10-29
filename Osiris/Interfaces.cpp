#include <stdexcept>

#define WIN32_LEAN_AND_MEAN
#include <Windows.h>

#include "xorstr.hpp"

#include "Interfaces.h"

Interfaces::Interfaces()
{
    try {
        engineClient = find<EngineClient>(xorstr_("engine.dll"), xorstr_("VEngineClient"));
        clientEntityList = find<ClientEntityList>(xorstr_("client_panorama.dll"), xorstr_("VClientEntityList"));
        client = find<Client>(xorstr_("client_panorama.dll"), xorstr_("VClient"));
        surface = find<Surface>(xorstr_("vguimatsurface.dll"), xorstr_("VGUI_Surface"));
        cvar = find<Cvar>(xorstr_("vstdlib.dll"), xorstr_("VEngineCvar"));
    }
    catch (std::runtime_error& e) {
        MessageBox(NULL, e.what(), xorstr_("Error"), MB_OK | MB_ICONERROR);
        std::exit(EXIT_FAILURE);
    }
}
