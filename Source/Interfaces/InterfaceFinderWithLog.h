#pragma once

#include <Platform/Macros/IsPlatform.h>

#if IS_WIN32()
#include <string>
#include <Platform/Windows/WindowsMessageBox.h>
#endif

#include "InterfaceFinder.h"

struct InterfaceFinderWithLog {
    explicit InterfaceFinderWithLog(InterfaceFinder finder)
        : finder{ finder }
    {
    }

    void* operator()(const char* name) const noexcept
    {
        if (const auto foundInterface = finder(name))
            return foundInterface;

#if IS_WIN32()
        WindowsMessageBox{}.showError("Osiris", ("Failed to find " + std::string{ name } + " interface!").c_str());
#endif
        std::exit(EXIT_FAILURE);
    }

private:
    InterfaceFinder finder;
};
