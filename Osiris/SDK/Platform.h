#pragma once

#ifdef _WIN32

#define __THISCALL __thiscall
#define __FASTCALL __fastcall
#define __CDECL __cdecl
#define __STDCALL __stdcall

#else

#define __THISCALL
#define __FASTCALL
#define __CDECL
#define __STDCALL

#endif

#ifdef _WIN32

#define CLIENT_DLL "client"
#define ENGINE_DLL "engine"
#define INPUTSYSTEM_DLL "inputsystem"
#define LOCALIZE_DLL "localize"
#define MATERIALSYSTEM_DLL "materialsystem"
#define SOUNDEMITTERSYSTEM_DLL "soundemittersystem"
#define STUDIORENDER_DLL "studiorender"
#define TIER0_DLL "tier0"
#define VGUI2_DLL "vgui2"
#define VGUIMATSURFACE_DLL "vguimatsurface"
#define VPHYSICS_DLL "vphysics"
#define VSTDLIB_DLL "vstdlib"

#elif __linux__

#define CLIENT_DLL "csgo/bin/linux64/client_client.so"
#define ENGINE_DLL "engine_client.so"
#define INPUTSYSTEM_DLL "inputsystem_client.so"
#define LOCALIZE_DLL "localize_client.so"
#define MATERIALSYSTEM_DLL "materialsystem_client.so"
#define SOUNDEMITTERSYSTEM_DLL "soundemittersystem_client.so"
#define STUDIORENDER_DLL "studiorender_client.so"
#define TIER0_DLL "libtier0_client.so"
#define VGUI2_DLL "vgui2_client.so"
#define VGUIMATSURFACE_DLL "vguimatsurface_client.so"
#define VPHYSICS_DLL "vphysics_client.so"
#define VSTDLIB_DLL "libvstdlib_client.so"

#endif

#ifdef _WIN32
#define LINUX_ARGS(...)
#define RETURN_ADDRESS() std::uintptr_t(_ReturnAddress())
#define FRAME_ADDRESS() (std::uintptr_t(_AddressOfReturnAddress()) - sizeof(std::uintptr_t))
#define IS_WIN32() true
#define WIN32_LINUX(win32, linux) win32
#else
#define LINUX_ARGS(...) __VA_ARGS__
#define RETURN_ADDRESS() std::uintptr_t(__builtin_return_address(0))
#define FRAME_ADDRESS() std::uintptr_t(__builtin_frame_address(0))
#define IS_WIN32() false
#define WIN32_LINUX(win32, linux) linux
#endif
