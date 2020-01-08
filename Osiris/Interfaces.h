#pragma once

#include <sstream>
#include <type_traits>
#include <Windows.h>

class Client;
class Cvar;
class Engine;
class EngineTrace;
class EntityList;
class GameEventManager;
class GameUI;
class InputSystem;
class Localize;
class MaterialSystem;
class ModelInfo;
class ModelRender;
class Panel;
class PhysicsSurfaceProps;
class RenderView;
class ResourceAccessControl;
class Surface;
class Sound;
class SoundEmitter;

class Interfaces {
public:
    Client* client = find<Client>(L"client_panorama", "VClient018");
    Cvar* cvar = find<Cvar>(L"vstdlib", "VEngineCvar007");
    Engine* engine = find<Engine>(L"engine", "VEngineClient014");
    EngineTrace* engineTrace = find<EngineTrace>(L"engine", "EngineTraceClient004");
    EntityList* entityList = find<EntityList>(L"client_panorama", "VClientEntityList003");;
    GameEventManager* gameEventManager = find<GameEventManager>(L"engine", "GAMEEVENTSMANAGER002");
    GameUI* gameUI = find<GameUI>(L"client_panorama", "GameUI011");
    InputSystem* inputSystem = find<InputSystem>(L"inputsystem", "InputSystemVersion001");
    Localize* localize = find<Localize>(L"localize", "Localize_001");
    MaterialSystem* materialSystem = find<MaterialSystem>(L"materialsystem", "VMaterialSystem080");
    ModelInfo* modelInfo = find<ModelInfo>(L"engine", "VModelInfoClient004");
    ModelRender* modelRender = find<ModelRender>(L"engine", "VEngineModel016");
    Panel* panel = find<Panel>(L"vgui2", "VGUI_Panel009");
    PhysicsSurfaceProps* physicsSurfaceProps = find<PhysicsSurfaceProps>(L"vphysics", "VPhysicsSurfaceProps001");
    RenderView* renderView = find<RenderView>(L"engine", "VEngineRenderView014");
    ResourceAccessControl* resourceAccessControl = find<ResourceAccessControl>(L"datacache", "VResourceAccessControl001");
    Surface* surface = find<Surface>(L"vguimatsurface", "VGUI_Surface031");
    Sound* sound = find<Sound>(L"engine", "IEngineSoundClient003");
    SoundEmitter* soundEmitter = find<SoundEmitter>(L"soundemittersystem", "VSoundEmitter003");
private:
    template <typename T>
    static auto find(const wchar_t* module, const char* name) noexcept
    {
        if (HMODULE moduleHandle = GetModuleHandleW(module))
            if (const auto createInterface = reinterpret_cast<std::add_pointer_t<T* (const char* name, int* returnCode)>>(GetProcAddress(moduleHandle, "CreateInterface")))
                if (T* foundInterface = createInterface(name, nullptr))
                    return foundInterface;

        MessageBoxA(nullptr, (std::ostringstream{ } << "Failed to find " << name << " interface!").str().c_str(), "Osiris", MB_OK | MB_ICONERROR);
        std::exit(EXIT_FAILURE);
    }
};

extern const Interfaces interfaces;
