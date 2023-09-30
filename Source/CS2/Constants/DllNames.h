#pragma once

#include <Platform/Macros/PlatformSpecific.h>

namespace cs2
{

constexpr auto CLIENT_DLL = WIN32_LINUX("client.dll", "libclient.so");
constexpr auto ENGINE_DLL = WIN32_LINUX("engine2.dll", "libengine2.so");
constexpr auto PANORAMA_DLL = WIN32_LINUX("panorama.dll", "libpanorama.so");
constexpr auto SDL_DLL = WIN32_LINUX("SDL3.dll", "libSDL3.so.0");

}
