#pragma once

#include <SDL/SdlConstants.h>
#include <SDL/SdlDll.h>
#include <SDL/SdlFunctions.h>

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
        if (showSimpleMessageBox)
            showSimpleMessageBox(flags, title, message, nullptr);
    }

    sdl3::SDL_ShowSimpleMessageBox* showSimpleMessageBox{SdlDll{}.showSimpleMessageBox()};
};
