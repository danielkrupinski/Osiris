#pragma once

#include <Windows.h>

#include "WindowsDynamicLibrary.h"

class WindowsMessageBox {
public:
    void showWarning(const char* title, const char* message) const noexcept
    {
        showMessageBox(title, message, MB_ICONWARNING);
    }

    void showError(const char* title, const char* message) const noexcept
    {
        showMessageBox(title, message, MB_ICONERROR);
    }

private:
    void showMessageBox(const char* title, const char* message, unsigned int extraFlags) const noexcept
    {
        if (messageBoxA)
            messageBoxA(nullptr, message, title, MB_OK | extraFlags);
    }

    decltype(&MessageBoxA) messageBoxA = WindowsDynamicLibrary{ "USER32.dll" }.getFunctionAddress("MessageBoxA").as<decltype(&MessageBoxA)>();
};
