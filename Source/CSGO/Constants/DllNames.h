#pragma once

#include <Platform/Macros/PlatformSpecific.h>

namespace csgo
{

constexpr auto CLIENT_DLL = WIN32_LINUX("client.dll", "csgo/bin/linux64/client_client.so");
constexpr auto ENGINE_DLL = WIN32_LINUX("engine.dll", "engine_client.so");
constexpr auto FILESYSTEM_DLL = WIN32_LINUX("filesystem_stdio.dll", "filesystem_stdio_client.so");
constexpr auto INPUTSYSTEM_DLL = WIN32_LINUX("inputsystem.dll", "inputsystem_client.so");
constexpr auto LOCALIZE_DLL = WIN32_LINUX("localize.dll", "localize_client.so");
constexpr auto MATERIALSYSTEM_DLL = WIN32_LINUX("materialsystem.dll", "materialsystem_client.so");
constexpr auto PANORAMA_DLL = WIN32_LINUX("panorama.dll", "panorama_gl_client.so");
constexpr auto SOUNDEMITTERSYSTEM_DLL = WIN32_LINUX("soundemittersystem.dll", "soundemittersystem_client.so");
constexpr auto STUDIORENDER_DLL = WIN32_LINUX("studiorender.dll", "studiorender_client.so");
constexpr auto TIER0_DLL = WIN32_LINUX("tier0.dll", "libtier0_client.so");
constexpr auto VGUI2_DLL = WIN32_LINUX("vgui2.dll", "vgui2_client.so");
constexpr auto VGUIMATSURFACE_DLL = WIN32_LINUX("vguimatsurface.dll", "vguimatsurface_client.so");
constexpr auto VPHYSICS_DLL = WIN32_LINUX("vphysics.dll", "vphysics_client.so");
constexpr auto VSTDLIB_DLL = WIN32_LINUX("vstdlib.dll", "libvstdlib_client.so");

}
