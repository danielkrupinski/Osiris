#include <Windows.h>

#include "Memory.h"
#include "SDK/IAppSystem.h"
#include "Interfaces.h"

Interfaces::Interfaces()
{
    engine = reinterpret_cast<IVEngineClient*>(find("engine.dll", "VEngineClient"));
    baseClient = reinterpret_cast<IBaseClientDLL*>(find("client_panorama.dll", "VClient"));
    globalVars = **reinterpret_cast<CGlobalVarsBase***>(memory.globalVars);
    localPlayer = *reinterpret_cast<C_LocalPlayer*>(memory.localPlayer);
}

void* Interfaces::find(const std::string& module, const std::string& name)
{
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
