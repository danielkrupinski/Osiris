#include <stdexcept>

#define WIN32_LEAN_AND_MEAN
#include <Windows.h>

#include "xorstr.hpp"

#include "Interfaces.h"

Interfaces::Interfaces() noexcept
{
    try {
        engineClient = find<EngineClient>(xorstr_("engine.dll"), xorstr_("VEngineClient"));
        clientEntityList = find<ClientEntityList>(xorstr_("client_panorama.dll"), xorstr_("VClientEntityList"));
        client = find<Client>(xorstr_("client_panorama.dll"), xorstr_("VClient"));
        surface = find<Surface>(xorstr_("vguimatsurface.dll"), xorstr_("VGUI_Surface"));
        cvar = find<Cvar>(xorstr_("vstdlib.dll"), xorstr_("VEngineCvar"));
        modelRender = find<ModelRender>(xorstr_("engine.dll"), xorstr_("VEngineModel"));
        materialSystem = find<MaterialSystem>(xorstr_("materialsystem.dll"), xorstr_("VMaterialSystem"));
        renderView = find<RenderView>(xorstr_("engine.dll"), xorstr_("VEngineRenderView"));
    }
    catch (const std::runtime_error& e) {
        MessageBox(NULL, e.what(), xorstr_("Error"), MB_OK | MB_ICONERROR);
        std::exit(EXIT_FAILURE);
    }
}
