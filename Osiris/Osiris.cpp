#include "Config.h"
#include "GUI.h"
#include "Hooks.h"
#include "Interfaces.h"
#include "Memory.h"
#include "Netvars.h"
#include "AntiDetection.h"

Config config{ "Osiris" };
GUI gui;
const Interfaces interfaces;
Memory memory;
// Netvars netvars;
//Hooks hooks;
AntiDetection antiDetection;
static HMODULE module;
static WNDPROC originalWndproc;

static LRESULT WINAPI init(HWND window, UINT msg, WPARAM wParam, LPARAM lParam) noexcept
{
    SetWindowLongPtr(FindWindowW(L"Valve001", nullptr), GWLP_WNDPROC, LONG_PTR(originalWndproc));
    netvars = std::make_unique<Netvars>();
    hooks = std::make_unique<Hooks>(module);

    return CallWindowProc(originalWndproc, window, msg, wParam, lParam);
}

BOOL APIENTRY DllMain(HMODULE hModule, DWORD ul_reason_for_call, LPVOID)
{
    if (ul_reason_for_call == DLL_PROCESS_ATTACH) {
        module = hModule;
        originalWndproc = WNDPROC(SetWindowLongPtr(FindWindowW(L"Valve001", nullptr), GWLP_WNDPROC, LONG_PTR(init)));
    }
    return TRUE;
}
