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
    Interfaces() noexcept;

    Client* client;
    Cvar* cvar;
    Engine* engine;
    EngineTrace* engineTrace;
    EntityList* entityList;
    GameEventManager* gameEventManager;
    GameUI* gameUI;
    InputSystem* inputSystem;
    Localize* localize;
    MaterialSystem* materialSystem;
    ModelInfo* modelInfo;
    ModelRender* modelRender;
    Panel* panel;
    PhysicsSurfaceProps* physicsSurfaceProps;
    RenderView* renderView;
    ResourceAccessControl* resourceAccessControl;
    Surface* surface;
    Sound* sound;
    SoundEmitter* soundEmitter;
private:
    template <typename T>
    static auto find(const wchar_t* module, const char* name)
    {
        if (const auto createInterface{ reinterpret_cast<std::add_pointer_t<T* (const char* name, int* returnCode)>>(GetProcAddress(GetModuleHandleW(module), "CreateInterface")) })
            if (T* foundInterface{ createInterface(name, nullptr) })
                return foundInterface;

        MessageBoxA(nullptr, (std::ostringstream{ } << "Failed to find " << name << " interface!").str().c_str(), "Osiris", MB_OK | MB_ICONERROR);
        std::exit(EXIT_FAILURE);
    }
};

extern Interfaces interfaces;
