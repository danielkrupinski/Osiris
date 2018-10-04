#pragma once

#include <functional>

#include <d3d9.h>

class Hooks {
public:
    Hooks();
private:
    std::function<HRESULT STDMETHODCALLTYPE(IDirect3DDevice9*, const RECT*, const RECT*, HWND, const RGNDATA*)> originalPresent;
    HRESULT STDMETHODCALLTYPE hookedPresent(IDirect3DDevice9*, const RECT*, const RECT*, HWND, const RGNDATA*);
    std::function<HRESULT STDMETHODCALLTYPE(IDirect3DDevice9*, D3DPRESENT_PARAMETERS*)> originalReset;
};