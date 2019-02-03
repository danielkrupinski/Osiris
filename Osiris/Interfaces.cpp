#include <stdexcept>
#include <Windows.h>

#include "Interfaces.h"

Interfaces::Interfaces() noexcept
{
    try {
        engine = find<Engine>("engine", "VEngineClient");
        entityList = find<EntityList>("client_panorama", "VClientEntityList");
        client = find<Client>("client_panorama", "VClient");
        cvar = find<Cvar>("vstdlib", "VEngineCvar");
        modelRender = find<ModelRender>("engine", "VEngineModel");
        materialSystem = find<MaterialSystem>("materialsystem", "VMaterialSystem");
        renderView = find<RenderView>("engine", "VEngineRenderView");
        inputSystem = find<InputSystem>("inputsystem", "InputSystemVersion");
    }
    catch (const std::runtime_error& e) {
        MessageBox(NULL, e.what(), "Error", MB_OK | MB_ICONERROR);
        exit(EXIT_FAILURE);
    }
}
