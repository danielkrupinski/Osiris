#include "imgui/imgui.h"
#include "imgui/imgui_impl_dx9.h"
#include "imgui/imgui_impl_win32.h"

#include "Hooks.h"
#include "Memory.h"

Hooks::Hooks()
{
    originalPresent = reinterpret_cast<HRESULT(STDMETHODCALLTYPE*)(IDirect3DDevice9*, const RECT*, const RECT*, HWND, const RGNDATA*)>(memory.present);
    originalReset = reinterpret_cast<HRESULT(STDMETHODCALLTYPE*)(IDirect3DDevice9*, D3DPRESENT_PARAMETERS*)>(memory.reset);
}

LRESULT STDMETHODCALLTYPE Hooks::hookedWndProc(HWND window, UINT msg, WPARAM wParam, LPARAM lParam)
{
    ImGuiIO& io = ImGui::GetIO();
    switch (msg) {
    case WM_LBUTTONDOWN:
        io.MouseDown[0] = true;
        return true;
    case WM_LBUTTONUP:
        io.MouseDown[0] = false;
        return true;
    case WM_RBUTTONDOWN:
        io.MouseDown[1] = true;
        return true;
    case WM_RBUTTONUP:
        io.MouseDown[1] = false;
        return true;
    case WM_MBUTTONDOWN:
        io.MouseDown[2] = true;
        return true;
    case WM_MBUTTONUP:
        io.MouseDown[2] = false;
        return true;
    case WM_MOUSEWHEEL:
        io.MouseWheel += GET_WHEEL_DELTA_WPARAM(wParam) > 0 ? +1.0f : -1.0f;
        return true;
    case WM_MOUSEMOVE:
        io.MousePos.x = (signed short)(lParam);
        io.MousePos.y = (signed short)(lParam >> 16);
        return true;
    case WM_KEYDOWN:
        if (wParam < 256)
            io.KeysDown[wParam] = 1;
        return true;
    case WM_KEYUP:
        if (wParam < 256)
            io.KeysDown[wParam] = 0;
        return true;
    case WM_CHAR:
        if (wParam > 0 && wParam < 0x10000)
            io.AddInputCharacter((unsigned short)wParam);
        return true;
    }
    return CallWindowProc(originalWndProc, window, msg, wParam, lParam);
}

HRESULT STDMETHODCALLTYPE Hooks::hookedPresent(IDirect3DDevice9* device, const RECT* src, const RECT* dest, HWND windowOverride, const RGNDATA* dirtyRegion)
{
    static bool isInitialised{ false };

    if (!isInitialised) {
        originalWndProc = reinterpret_cast<WNDPROC>(
            SetWindowLongPtr(FindWindowA("Valve001", NULL), GWLP_WNDPROC, LONG_PTR(this->hookedWndProc))
            );

        ImGui_ImplWin32_Init(FindWindowA("Valve001", NULL));
        ImGui_ImplDX9_Init(device);
        isInitialised = true;
    } else {
        ImGui_ImplDX9_NewFrame();
        ImGui::Begin("Test window");
        ImGui::End();
        ImGui::Render();
    }
    return originalPresent(device, src, dest, windowOverride, dirtyRegion);
}

HRESULT STDMETHODCALLTYPE Hooks::hookedReset(IDirect3DDevice9* device, D3DPRESENT_PARAMETERS* params)
{
    ImGui_ImplDX9_InvalidateDeviceObjects();
    ImGui_ImplDX9_CreateDeviceObjects();
    return originalReset(device, params);
}
