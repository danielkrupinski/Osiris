#pragma once

#include <Windows.h>

#include "DynamicLibraryView.h"

namespace windows_platform
{

template <typename DynamicLibraryWrapper>
class DynamicLibrary {
public:
    DynamicLibrary(DynamicLibraryWrapper dynamicLibraryWrapper, const char* libraryName)
        : dl{ dynamicLibraryWrapper }, handle{ dl.GetModuleHandleA(libraryName) }
    {
    }

    [[nodiscard]] DynamicLibraryView<DynamicLibraryWrapper> getView() const noexcept
    {
        return { dl, handle };
    }

private:
    [[no_unique_address]] DynamicLibraryWrapper dl;
    HMODULE handle;
};

}
