#include <stdexcept>
#include <Windows.h>

#include "Interfaces.h"

Interfaces::Interfaces() noexcept
{
    try {
        client = find<Client>("client_panorama", "VClient018");
        cvar = find<Cvar>("vstdlib", "VEngineCvar007");
        engine = find<Engine>("engine", "VEngineClient014");
        engineTrace = find<EngineTrace>("engine", "EngineTraceClient004");
        entityList = find<EntityList>("client_panorama", "VClientEntityList003");
        gameUI = find<GameUI>("client_panorama", "GameUI011");
        inputSystem = find<InputSystem>("inputsystem", "InputSystemVersion001");
        materialSystem = find<MaterialSystem>("materialsystem", "VMaterialSystem080");
        modelInfo = find<ModelInfo>("engine", "VModelInfoClient004");
        modelRender = find<ModelRender>("engine", "VEngineModel016");
        panel = find<Panel>("vgui2", "VGUI_Panel009");
        renderView = find<RenderView>("engine", "VEngineRenderView014");
        surface = find<Surface>("vguimatsurface", "VGUI_Surface031");
        sound = find<Sound>("engine", "IEngineSoundClient003");
    }
    catch (const std::runtime_error& e) {
        MessageBox(NULL, e.what(), "Error", MB_OK | MB_ICONERROR);
        exit(EXIT_FAILURE);
    }
}
