#pragma once

#include <CS2/Constants/DllNames.h>
#include "LinuxDynamicLibrary.h"

namespace sdl3
{

struct SDL_Window;
using SDL_ShowSimpleMessageBox = int(std::uint32_t flags, const char* title, const char* message, SDL_Window* window);

constexpr auto SDL_MESSAGEBOX_ERROR = 0x10;
constexpr auto SDL_MESSAGEBOX_WARNING = 0x20;

}

class LinuxMessageBox {
public:
    void showWarning(const char* title, const char* message) const noexcept
    {
        showMessageBox(title, message, sdl3::SDL_MESSAGEBOX_WARNING);
    }

    void showError(const char* title, const char* message) const noexcept
    {
        showMessageBox(title, message, sdl3::SDL_MESSAGEBOX_ERROR);
    }

private:
    void showMessageBox(const char* title, const char* message, unsigned int flags) const noexcept
    {
        /* NOTE: doesn't work after game devs switched SDL to prefer Wayland, need to wait until they fix it
        if (showSimpleMessageBox)
            showSimpleMessageBox(flags, title, message, nullptr);
        */
    }

    sdl3::SDL_ShowSimpleMessageBox* showSimpleMessageBox = LinuxDynamicLibrary{ cs2::SDL_DLL }.getFunctionAddress("SDL_ShowSimpleMessageBox").as<sdl3::SDL_ShowSimpleMessageBox*>();
};
