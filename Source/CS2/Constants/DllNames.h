#pragma once

#include <Platform/Macros/PlatformSpecific.h>

namespace cs2
{

constexpr auto CLIENT_DLL = WIN32_LINUX("client.dll", "libclient.so");
constexpr auto ENGINE_DLL = WIN32_LINUX("engine2.dll", "libengine2.so");
constexpr auto FILESYSTEM_DLL = WIN32_LINUX("filesystem_stdio.dll", "libfilesystem_stdio.so");
constexpr auto PANORAMA_DLL = WIN32_LINUX("panorama.dll", "libpanorama.so");
constexpr auto SDL_DLL = WIN32_LINUX("SDL3.dll", "libSDL3.so.0");
constexpr auto SOUNDSYSTEM_DLL = WIN32_LINUX("soundsystem.dll", "libsoundsystem.so");
constexpr auto TIER0_DLL = WIN32_LINUX("tier0.dll", "libtier0.so");

}
