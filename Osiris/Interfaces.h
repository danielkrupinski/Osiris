#pragma once

#include <iomanip>
#include <sstream>
#include <stdexcept>
#include <string>
#include <type_traits>
#include <Windows.h>

#include "SDK/Client.h"
#include "SDK/Cvar.h"
#include "SDK/EntityList.h"
#include "SDK/Engine.h"
#include "SDK/EngineTrace.h"
#include "SDK/InputSystem.h"
#include "SDK/MaterialSystem.h"
#include "SDK/ModelInfo.h"
#include "SDK/ModelRender.h"
#include "SDK/RenderView.h"

class Interfaces final {
public:
    Interfaces() noexcept;

    Engine* engine;
    EngineTrace* engineTrace;
    EntityList* entityList;
    Client* client;
    Cvar* cvar;
    ModelInfo* modelInfo;
    ModelRender* modelRender;
    MaterialSystem* materialSystem;
    RenderView* renderView;
    InputSystem* inputSystem;
private:
    template <typename T>
    auto find(const std::string& module, const std::string& name) const
    {
        const auto createInterface = reinterpret_cast<std::add_pointer_t<T* (const char* name, int* returnCode)>>(GetProcAddress(GetModuleHandleA(module.c_str()), "CreateInterface"));

        if (auto foundInterface = createInterface(name.c_str(), nullptr))
            return foundInterface;
        else
            throw std::runtime_error("Could not find " + name + " in " + module + "!");
    }
};

extern Interfaces interfaces;
