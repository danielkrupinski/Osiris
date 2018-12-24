#pragma once

#include <iomanip>
#include <sstream>
#include <stdexcept>
#include <string>
#include <type_traits>
#include <Windows.h>

#include "SDK/EntityList.h"
#include "SDK/Cvar.h"
#include "SDK/Engine.h"
#include "SDK/InputSystem.h"
#include "SDK/MaterialSystem.h"
#include "SDK/ModelRender.h"
#include "SDK/RenderView.h"
#include "SDK/Surface.h"

class Interfaces final {
public:
    explicit Interfaces() noexcept;

    Engine* engine;
    EntityList* entityList;
    class Client* client;
    Surface* surface;
    Cvar* cvar;
    ModelRender* modelRender;
    MaterialSystem* materialSystem;
    RenderView* renderView;
    InputSystem* inputSystem;
private:
    template <typename T>
    auto find(const std::string& module, const std::string& name) const
    {
        const auto createInterface = reinterpret_cast<std::add_pointer_t<T*(const char* name, int* returnCode)>>(GetProcAddress(GetModuleHandleA(module.c_str()), "CreateInterface"));

        for (int i = 1; i < 100; i++)
            if (T* foundInterface = createInterface((std::stringstream{ } << name << std::setfill('0') << std::setw(3) << i).str().c_str(), nullptr))
                return foundInterface;

        throw std::runtime_error("Could not find " + name + " in " + module + "!");
    }
};

extern Interfaces interfaces;
