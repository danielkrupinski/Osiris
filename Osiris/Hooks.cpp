#include "imgui/imgui.h"
#include "imgui/imgui_impl_dx9.h"

#include "Hooks.h"
#include "Memory.h"

Hooks::Hooks()
{
    originalPresent = reinterpret_cast<HRESULT(STDMETHODCALLTYPE*)(IDirect3DDevice9*, const RECT*, const RECT*, HWND, const RGNDATA*)>(memory.present);
    originalReset = reinterpret_cast<HRESULT(STDMETHODCALLTYPE*)(IDirect3DDevice9*, D3DPRESENT_PARAMETERS*)>(memory.reset);
}

LRESULT STDMETHODCALLTYPE hookedWndProc(HWND window, UINT messageType, WPARAM wParam, LPARAM lParam)
{

}
