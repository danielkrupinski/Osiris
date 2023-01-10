#pragma once

#include <Platform/Macros/IsPlatform.h>

#if IS_WIN32()
#include <Windows.h>
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
        MessageBoxA(nullptr, ("找不到 " + std::string{ name } + " 接口!").c_str(), "QS", MB_OK | MB_ICONERROR);
#endif
        std::exit(EXIT_FAILURE);
    }

private:
    InterfaceFinder finder;
};
