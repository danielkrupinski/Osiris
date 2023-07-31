#pragma once

#include <combaseapi.h>

#include <Platform/Windows/WindowsDynamicLibrary.h>

struct CombaseDll : WindowsDynamicLibrary {
    CombaseDll() : WindowsDynamicLibrary{ "combase.dll" }
    {
    }

    [[nodiscard]] auto CoTaskMemFree() const noexcept
    {
        return getFunctionAddress("CoTaskMemFree").as<decltype(&::CoTaskMemFree)>();
    }
};
