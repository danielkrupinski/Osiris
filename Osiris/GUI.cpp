#include "imgui/imgui.h"
#include "imgui/imgui_impl_dx9.h"
#include "imgui/imgui_impl_win32.h"

#include "GUI.h"

GUI::GUI(HWND window, IDirect3DDevice9* device)
{
    ImGui_ImplWin32_Init(window);
    ImGui_ImplDX9_Init(device);
}
