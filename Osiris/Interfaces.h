#pragma once

#include <stdexcept>
#include <type_traits>
#include <Windows.h>

#include "SDK/Client.h"
#include "SDK/Cvar.h"
#include "SDK/Engine.h"
#include "SDK/EngineTrace.h"
#include "SDK/EntityList.h"
#include "SDK/GameUI.h"
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
    GameUI* gameUI;
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
    static auto find(const char* module, const char* name)
    {
        const auto createInterface = reinterpret_cast<std::add_pointer_t<T* (const char* name, int* returnCode)>>(GetProcAddress(GetModuleHandleA(module), "CreateInterface"));

        if (auto foundInterface = createInterface(name, nullptr))
            return foundInterface;
        else
            throw std::runtime_error{ "Interface search failed!" };
    }
};

extern Interfaces interfaces;
