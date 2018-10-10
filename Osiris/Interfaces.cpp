#include <type_traits>
#include <Windows.h>

#include "Interfaces.h"

Interfaces::Interfaces()
{
    engineClient = reinterpret_cast<EngineClient*>(find("engine.dll", "VEngineClient"));
    clientEntityList = reinterpret_cast<ClientEntityList*>(find("client_panorama.dll", "VClientEntityList"));
    client = reinterpret_cast<Client*>(find("client_panorama.dll", "VClient"));
}

void* Interfaces::find(const std::string& module, const std::string& name)
{
    void* foundInterface = nullptr;
    std::add_pointer_t<void* (const char* name, int* returnCode)> createInterface = reinterpret_cast<decltype(createInterface)>(GetProcAddress(GetModuleHandleA(module.c_str()), "CreateInterface"));

    for (int i = 1; i < 100; i++) {
        std::string interfaceName{ name + std::string{ '0' } + std::to_string(i) };
        foundInterface = (void*)createInterface(interfaceName.c_str(), nullptr);
        if (foundInterface)
            break;

        interfaceName = name + "00" + std::to_string(i);
        foundInterface = (void*)createInterface(interfaceName.c_str(), nullptr);
        if (foundInterface)
            break;
    }
    return foundInterface;
}
