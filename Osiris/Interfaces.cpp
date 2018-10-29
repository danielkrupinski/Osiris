#include <stdexcept>
#include <type_traits>

#define WIN32_LEAN_AND_MEAN
#include <Windows.h>

#include "xorstr.hpp"

#include "Interfaces.h"

Interfaces::Interfaces()
{
    try {
        engineClient = reinterpret_cast<EngineClient*>(find(xorstr_("engine.dll"), xorstr_("VEngineClient")));
        clientEntityList = reinterpret_cast<ClientEntityList*>(find(xorstr_("client_panorama.dll"), xorstr_("VClientEntityList")));
        client = reinterpret_cast<Client*>(find(xorstr_("client_panorama.dll"), xorstr_("VClient")));
        surface = reinterpret_cast<Surface*>(find(xorstr_("vguimatsurface.dll"), xorstr_("VGUI_Surface")));
        cvar = reinterpret_cast<Cvar*>(find(xorstr_("vstdlib.dll"), xorstr_("VEngineCvar")));
    }
    catch (std::runtime_error& e) {
        MessageBox(NULL, e.what(), xorstr_("Error"), MB_OK | MB_ICONERROR);
        std::exit(EXIT_FAILURE);
    }
}
