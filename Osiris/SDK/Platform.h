#pragma once

#ifdef _WIN32

#define THISCALL_CONV __thiscall
#define FASTCALL_CONV __fastcall
#define CDECL_CONV __cdecl
#define STDCALL_CONV __stdcall

#else

#define THISCALL_CONV
#define FASTCALL_CONV
#define CDECL_CONV
#define STDCALL_CONV

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

constexpr auto CLIENT_DLL = WIN32_LINUX("client", "csgo/bin/linux64/client_client.so");
constexpr auto ENGINE_DLL = WIN32_LINUX("engine", "engine_client.so");
constexpr auto FILESYSTEM_DLL = WIN32_LINUX("filesystem_stdio", "filesystem_stdio_client.so");
constexpr auto INPUTSYSTEM_DLL = WIN32_LINUX("inputsystem", "inputsystem_client.so");
constexpr auto LOCALIZE_DLL = WIN32_LINUX("localize", "localize_client.so");
constexpr auto MATERIALSYSTEM_DLL = WIN32_LINUX("materialsystem", "materialsystem_client.so");
constexpr auto PANORAMA_DLL = WIN32_LINUX("panorama", "panorama_gl_client.so");
constexpr auto SOUNDEMITTERSYSTEM_DLL = WIN32_LINUX("soundemittersystem", "soundemittersystem_client.so");
constexpr auto STUDIORENDER_DLL = WIN32_LINUX("studiorender", "studiorender_client.so");
constexpr auto TIER0_DLL = WIN32_LINUX("tier0", "libtier0_client.so");
constexpr auto VGUI2_DLL = WIN32_LINUX("vgui2", "vgui2_client.so");
constexpr auto VGUIMATSURFACE_DLL = WIN32_LINUX("vguimatsurface", "vguimatsurface_client.so");
constexpr auto VPHYSICS_DLL = WIN32_LINUX("vphysics", "vphysics_client.so");
constexpr auto VSTDLIB_DLL = WIN32_LINUX("vstdlib", "libvstdlib_client.so");
