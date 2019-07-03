#pragma once

#include <sstream>
#include <stdexcept>
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
class Surface;
class Sound;
class SoundEmitter;

class Interfaces final {
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
    Surface* surface;
    Sound* sound;
    SoundEmitter* soundEmitter;
private:
    template <typename T>
    static auto find(const wchar_t* module, const char* name)
    {
        const auto createInterface = reinterpret_cast<std::add_pointer_t<T* (const char* name, int* returnCode)>>(GetProcAddress(GetModuleHandleW(module), "CreateInterface"));

        T* foundInterface{ nullptr };

        if (createInterface)
            foundInterface = createInterface(name, nullptr);

        if (foundInterface)
            return foundInterface;
        else {
            MessageBox(NULL, (std::ostringstream{ } << "Failed to find " << name << " in " << module << '!').str().c_str(), "Error", MB_OK | MB_ICONERROR);
            exit(EXIT_FAILURE);
        }
    }
};

extern Interfaces interfaces;
