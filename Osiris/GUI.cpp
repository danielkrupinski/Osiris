#include "imgui/imgui.h"
#include "imgui/imgui_impl_dx9.h"
#include "imgui/imgui_impl_win32.h"

#include "GUI.h"

GUI::GUI(IDirect3DDevice9* device, HWND window)
{
    ImGui_ImplDX9_Init(device);
}
