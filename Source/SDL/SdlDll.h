#pragma once

#include <CS2/Constants/DllNames.h>
#include <Platform/DynamicLibrary.h>
#include <SDL/SdlFunctions.h>

struct SdlDll : DynamicLibrary {
    SdlDll() noexcept
        : DynamicLibrary{cs2::SDL_DLL}
    {
    }

    [[nodiscard]] sdl3::SDL_PeepEvents* peepEvents() const noexcept
    {
        return getFunctionAddress("SDL_PeepEvents").as<sdl3::SDL_PeepEvents*>();
    }

    [[nodiscard]] sdl3::SDL_ShowSimpleMessageBox* showSimpleMessageBox() const noexcept
    {
        return getFunctionAddress("SDL_ShowSimpleMessageBox").as<sdl3::SDL_ShowSimpleMessageBox*>();
    }
};
