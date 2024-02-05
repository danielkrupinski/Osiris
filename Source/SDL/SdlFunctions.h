#pragma once

#include <cstdint>

namespace sdl3
{

struct SDL_Window;
using SDL_ShowSimpleMessageBox = int(std::uint32_t flags, const char* title, const char* message, SDL_Window* window);
using SDL_PeepEvents = int(void* events, int numevents, int action, unsigned minType, unsigned maxType);

}
