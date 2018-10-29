#pragma once

#include <stdexcept>
#include <string>
#include <type_traits>

#include "SDK/Client.h"
#include "SDK/ClientEntityList.h"
#include "SDK/Cvar.h"
#include "SDK/EngineClient.h"
#include "SDK/Material.h"
#include "SDK/Surface.h"

class Interfaces final {
public:
    Interfaces();

    EngineClient* engineClient;
    ClientEntityList* clientEntityList;
    Client* client;
    Surface* surface;
    Cvar* cvar;
private:
    template <typename T>
    auto find(const std::string& module, const std::string& name)
    {
        T* foundInterface = nullptr;
        std::add_pointer_t<T* (const char* name, int* returnCode)> createInterface = reinterpret_cast<decltype(createInterface)>(GetProcAddress(GetModuleHandleA(module.c_str()), xorstr_("CreateInterface")));

        for (int i = 1; i < 100; i++) {
            std::string interfaceName{ name + std::string{ xorstr_("0") } + std::to_string(i) };
            foundInterface = createInterface(interfaceName.c_str(), nullptr);
            if (foundInterface)
                break;

            interfaceName = name + xorstr_("00") + std::to_string(i);
            foundInterface = createInterface(interfaceName.c_str(), nullptr);
            if (foundInterface)
                break;
        }

        if (!foundInterface)
            throw std::runtime_error(std::string{ xorstr_("Could not find ") } + name + std::string{ xorstr_(" in ") } + module + std::string{ xorstr_("!") });
        return foundInterface;
    }

};

extern Interfaces interfaces;
