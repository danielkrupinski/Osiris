#pragma once

#include <iomanip>
#include <sstream>
#include <stdexcept>
#include <string>
#include <type_traits>

#include "xorstr.hpp"

#include "SDK/Client.h"
#include "SDK/ClientEntityList.h"
#include "SDK/Cvar.h"
#include "SDK/EngineClient.h"
#include "SDK/Material.h"
#include "SDK/Surface.h"

class Interfaces final {
public:
    explicit Interfaces() noexcept;

    EngineClient* engineClient;
    ClientEntityList* clientEntityList;
    Client* client;
    Surface* surface;
    Cvar* cvar;
private:
    template <typename T>
    auto find(const std::string& module, const std::string& name)
    {
        auto createInterface = reinterpret_cast<std::add_pointer_t<T* (const char* name, int* returnCode)>>(GetProcAddress(GetModuleHandleA(module.c_str()), xorstr_("CreateInterface")));

        for (int i = 1; i < 100; i++) {
            std::stringstream interfaceName;
            interfaceName << name << std::setfill('0') << std::setw(3) << i;
            if (T* foundInterface = createInterface(interfaceName.str().c_str(), nullptr))
                return foundInterface;
        }
        throw std::runtime_error(std::string{ xorstr_("Could not find ") } + name + std::string{ xorstr_(" in ") } + module + std::string{ xorstr_("!") });
    }

};

extern Interfaces interfaces;
