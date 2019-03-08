#pragma once

#include <iomanip>
#include <sstream>
#include <stdexcept>
#include <string>
#include <type_traits>
#include <Windows.h>

#include "SDK/Client.h"
#include "SDK/Cvar.h"
#include "SDK/Engine.h"
#include "SDK/EngineTrace.h"
#include "SDK/EntityList.h"
#include "SDK/InputSystem.h"
#include "SDK/MaterialSystem.h"
#include "SDK/ModelInfo.h"
#include "SDK/ModelRender.h"
#include "SDK/Panel.h"
#include "SDK/RenderView.h"
#include "SDK/Surface.h"

class Interfaces final {
public:
    Interfaces() noexcept;

    Client* client;
    Cvar* cvar;
    Engine* engine;
    EngineTrace* engineTrace;
    EntityList* entityList;
    InputSystem* inputSystem;
    MaterialSystem* materialSystem;
    ModelInfo* modelInfo;
    ModelRender* modelRender;
    Panel* panel;
    RenderView* renderView;
    Surface* surface;
    class Sound* sound;
private:
    template <typename T>
    static auto find(const std::string& module, const std::string& name)
    {
        const auto createInterface = reinterpret_cast<std::add_pointer_t<T* (const char* name, int* returnCode)>>(GetProcAddress(GetModuleHandleA(module.c_str()), "CreateInterface"));

        if (auto foundInterface = createInterface(name.c_str(), nullptr))
            return foundInterface;
        else
            throw std::runtime_error("Could not find " + name + " in " + module + "!");
    }
};

extern Interfaces interfaces;
