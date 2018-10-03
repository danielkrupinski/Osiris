#pragma once

#include <functional>

#include <d3d9.h>

class Hooks {
private:
    std::function<HRESULT STDMETHODCALLTYPE(IDirect3DDevice9*, const RECT*, const RECT*, HWND, const RGNDATA*)> originalPresent;
    std::function<HRESULT STDMETHODCALLTYPE(IDirect3DDevice9*, D3DPRESENT_PARAMETERS*)> originalReset;
};