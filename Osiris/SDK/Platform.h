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
#define VSTDLIB_DLL "vstdlib"
#define TIER0_DLL "tier0"

#elif __linux__

#define CLIENT_DLL "csgo/bin/linux64/client_client.so"
#define ENGINE_DLL "engine_client.so"
#define INPUTSYSTEM_DLL "inputsystem_client.so"
#define LOCALIZE_DLL "localize_client.so"
#define VSTDLIB_DLL "libvstdlib_client.so"
#define TIER0_DLL "libtier0_client.so"

#endif
