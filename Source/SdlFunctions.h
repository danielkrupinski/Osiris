#pragma once

#include <cstdint>

#include "Platform/Linux/DynamicLibraryWrapper.h"
#include "Platform/Linux/SharedObject.h"

template <typename T>
constexpr auto relativeToAbsolute(std::uintptr_t address) noexcept
{
    return (T)(address + 4 + *reinterpret_cast<std::int32_t*>(address));
}

struct SdlFunctions {
    SdlFunctions(linux_platform::SharedObject<linux_platform::DynamicLibraryWrapper> libSDL)
        : pollEvent{ relativeToAbsolute<std::uintptr_t>(std::uintptr_t(libSDL.getFunctionAddress("SDL_PollEvent")) + 2) },
        swapWindow{ relativeToAbsolute<std::uintptr_t>(std::uintptr_t(libSDL.getFunctionAddress("SDL_GL_SwapWindow")) + 2) }
    {
    }

    std::uintptr_t pollEvent = 0;
    std::uintptr_t swapWindow = 0;
};
