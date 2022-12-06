#pragma once

#include <cstdint>

#include "Platform/Linux/DynamicLibraryWrapper.h"
#include "Platform/Linux/DynamicLibraryView.h"

#include "Utils/SafeAddress.h"

struct SdlFunctions {
    SdlFunctions(linux_platform::DynamicLibraryView<linux_platform::DynamicLibraryWrapper> libSDL)
        : pollEvent{ SafeAddress{ std::uintptr_t(libSDL.getFunctionAddress("SDL_PollEvent")) }.add(2).relativeToAbsolute().get() },
          swapWindow{ SafeAddress{ std::uintptr_t(libSDL.getFunctionAddress("SDL_GL_SwapWindow")) }.add(2).relativeToAbsolute().get() }
    {
    }

    std::uintptr_t pollEvent = 0;
    std::uintptr_t swapWindow = 0;
};
