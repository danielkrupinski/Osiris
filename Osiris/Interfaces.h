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
class GameUI;
class InputSystem;
class Localize;
class MaterialSystem;
class ModelInfo;
class ModelRender;
class Panel;
class RenderView;
class Surface;
class Sound;

class Interfaces final {
public:
    Interfaces() noexcept;

    Client* client;
    Cvar* cvar;
    Engine* engine;
    EngineTrace* engineTrace;
    EntityList* entityList;
    GameUI* gameUI;
    InputSystem* inputSystem;
    Localize* localize;
    MaterialSystem* materialSystem;
    ModelInfo* modelInfo;
    ModelRender* modelRender;
    Panel* panel;
    RenderView* renderView;
    Surface* surface;
    Sound* sound;
private:
    template <typename T>
    static auto find(const char* module, const char* name)
    {
        const auto createInterface = reinterpret_cast<std::add_pointer_t<T* (const char* name, int* returnCode)>>(GetProcAddress(GetModuleHandleA(module), "CreateInterface"));

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
