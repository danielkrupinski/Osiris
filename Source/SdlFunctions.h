#pragma once

#include <cstdint>

#include "Platform/DynamicLibrary.h"
#include "Utils/SafeAddress.h"

struct SdlFunctions {
    template <typename PlatformApi>
    SdlFunctions(DynamicLibrary<PlatformApi> libSDL)
        : pollEvent{ libSDL.getFunctionAddress("SDL_PollEvent").add(2).relativeToAbsolute().get() },
          swapWindow{ libSDL.getFunctionAddress("SDL_GL_SwapWindow").add(2).relativeToAbsolute().get() }
    {
    }

    std::uintptr_t pollEvent = 0;
    std::uintptr_t swapWindow = 0;
};
