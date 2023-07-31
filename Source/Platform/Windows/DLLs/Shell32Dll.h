#pragma once

#include <ShlObj_core.h>

#include <Platform/Windows/WindowsDynamicLibrary.h>

struct Shell32Dll : WindowsDynamicLibrary {
    Shell32Dll() : WindowsDynamicLibrary{ "shell32.dll" }
    {
    }

    [[nodiscard]] auto SHGetKnownFolderPath() const noexcept
    {
        return getFunctionAddress("SHGetKnownFolderPath").as<decltype(&::SHGetKnownFolderPath)>();
    }
};
