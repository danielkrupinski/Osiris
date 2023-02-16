#pragma once

#include <Platform/Macros/CallingConventions.h>
#include <Platform/Macros/IsPlatform.h>

#if IS_WIN32()
#include <d3d9.h>
#include <Windows.h>
#elif IS_LINUX()
#include <SDL2/SDL.h>
#endif

void initializeGlobalContext();

#if IS_WIN32()

LRESULT __stdcall wndProc(HWND window, UINT msg, WPARAM wParam, LPARAM lParam) noexcept;
HRESULT __stdcall reset(IDirect3DDevice9* device, D3DPRESENT_PARAMETERS* params) noexcept;
HRESULT __stdcall present(IDirect3DDevice9* device, const RECT* src, const RECT* dest, HWND windowOverride, const RGNDATA* dirtyRegion) noexcept;

#elif IS_LINUX()

int pollEvent(SDL_Event* event) noexcept;
void swapWindow(SDL_Window* window) noexcept;

#endif

#if IS_WIN32()
DWORD WINAPI unload(HMODULE moduleHandle) noexcept;
#endif
