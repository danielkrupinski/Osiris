#pragma once

#include <SDL2/SDL.h>

#include "LinuxDynamicLibrary.h"

class LinuxMessageBox {
public:
    void showWarning(const char* title, const char* message) const noexcept
    {
        showMessageBox(title, message, SDL_MESSAGEBOX_WARNING);
    }

    void showError(const char* title, const char* message) const noexcept
    {
        showMessageBox(title, message, SDL_MESSAGEBOX_ERROR);
    }

private:
    void showMessageBox(const char* title, const char* message, unsigned int flags) const noexcept
    {
        if (showSimpleMessageBox)
            showSimpleMessageBox(flags, title, message, nullptr);
    }

    decltype(&SDL_ShowSimpleMessageBox) showSimpleMessageBox = LinuxDynamicLibrary{ "libSDL2-2.0.so.0" }.getFunctionAddress("SDL_ShowSimpleMessageBox").as<decltype(&SDL_ShowSimpleMessageBox)>();
};
