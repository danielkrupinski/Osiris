#include <Windows.h>

#include "Interfaces.h"

Interfaces::Interfaces()
{
    engine = reinterpret_cast<CEngine*>(find("engine.dll", "VEngineClient"));
}

void* Interfaces::find(const std::string& module, const std::string& name)
{
    typedef void* (*CreateInterfaceFn)(const char* Name, int* ReturnCode);

    void* Interface = nullptr;

    CreateInterfaceFn createInterface = (CreateInterfaceFn)GetProcAddress(GetModuleHandleA(module.c_str()), "CreateInterface");

    for (int i = 1; i < 100; i++) {
        std::string interfaceName{ name + std::string{ '0' } + std::to_string(i) };
        Interface = (void*)createInterface(interfaceName.c_str(), nullptr);
        if (Interface)
            break;

        interfaceName = name + "00" + std::to_string(i);
        Interface = (void*)createInterface(interfaceName.c_str(), nullptr);
        if (Interface)
            break;
    }
    return Interface;
}
