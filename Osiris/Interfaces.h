#pragma once

#include <iomanip>
#include <sstream>
#include <stdexcept>
#include <string>
#include <type_traits>
#define WIN32_LEAN_AND_MEAN
#include <Windows.h>

#include "xorstr.hpp"

#include "SDK/Client.h"
#include "SDK/ClientEntityList.h"
#include "SDK/Cvar.h"
#include "SDK/EngineClient.h"
#include "SDK/MaterialSystem.h"
#include "SDK/ModelRender.h"
#include "SDK/Surface.h"

class Interfaces final {
public:
    explicit Interfaces() noexcept;

    EngineClient* engineClient;
    ClientEntityList* clientEntityList;
    Client* client;
    Surface* surface;
    Cvar* cvar;
    ModelRender* modelRender;
private:
    template <typename T>
    auto find(const std::string& module, const std::string& name) const
    {
        auto createInterface = reinterpret_cast<std::add_pointer_t<T* (const char* name, int* returnCode)>>(GetProcAddress(GetModuleHandleA(module.c_str()), xorstr_("CreateInterface")));

        for (int i = 1; i < 100; i++) {
            if (T* foundInterface = createInterface((std::stringstream{ } << name << std::setfill('0') << std::setw(3) << i).str().c_str(), nullptr))
                return foundInterface;
        }
        throw std::runtime_error(xorstr_("Could not find ") + name + xorstr_(" in ") + module + xorstr_("!"));
    }
};

extern Interfaces interfaces;
