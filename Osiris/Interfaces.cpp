#include <stdexcept>
#include <Windows.h>

#include "Interfaces.h"

Interfaces::Interfaces() noexcept
{
    try {
        engine = find<Engine>("engine.dll", "VEngineClient");
        entityList = find<EntityList>("client_panorama.dll", "VClientEntityList");
        client = find<Client>("client_panorama.dll", "VClient");
        surface = find<Surface>("vguimatsurface.dll", "VGUI_Surface");
        cvar = find<Cvar>("vstdlib.dll", "VEngineCvar");
        modelRender = find<ModelRender>("engine.dll", "VEngineModel");
        materialSystem = find<MaterialSystem>("materialsystem.dll", "VMaterialSystem");
        renderView = find<RenderView>("engine.dll", "VEngineRenderView");
        inputSystem = find<InputSystem>("inputsystem.dll", "InputSystemVersion");
    }
    catch (const std::runtime_error& e) {
        MessageBox(NULL, e.what(), "Error", MB_OK | MB_ICONERROR);
        std::exit(EXIT_FAILURE);
    }
}
