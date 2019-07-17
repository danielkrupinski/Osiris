#include "Interfaces.h"

Interfaces::Interfaces() noexcept
{
    client = find<Client>(L"client_panorama", "VClient018");
    cvar = find<Cvar>(L"vstdlib", "VEngineCvar007");
    engine = find<Engine>(L"engine", "VEngineClient014");
    engineTrace = find<EngineTrace>(L"engine", "EngineTraceClient004");
    entityList = find<EntityList>(L"client_panorama", "VClientEntityList003");
    gameEventManager = find<GameEventManager>(L"engine", "GAMEEVENTSMANAGER002");
    gameUI = find<GameUI>(L"client_panorama", "GameUI011");
    inputSystem = find<InputSystem>(L"inputsystem", "InputSystemVersion001");
    localize = find<Localize>(L"localize", "Localize_001");
    materialSystem = find<MaterialSystem>(L"materialsystem", "VMaterialSystem080");
    modelInfo = find<ModelInfo>(L"engine", "VModelInfoClient004");
    modelRender = find<ModelRender>(L"engine", "VEngineModel016");
    panel = find<Panel>(L"vgui2", "VGUI_Panel009");
    physicsSurfaceProps = find<PhysicsSurfaceProps>(L"vphysics", "VPhysicsSurfaceProps001");
    renderView = find<RenderView>(L"engine", "VEngineRenderView014");
    resourceAccessControl = find<ResourceAccessControl>(L"datacache", "VResourceAccessControl001");
    surface = find<Surface>(L"vguimatsurface", "VGUI_Surface031");
    sound = find<Sound>(L"engine", "IEngineSoundClient003");
    soundEmitter = find<SoundEmitter>(L"soundemittersystem", "VSoundEmitter003");
}
