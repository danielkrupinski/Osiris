#pragma once

#include <Windows.h>

namespace windows_platform
{

template <typename DynamicLibraryWrapper>
class DynamicLibraryView {
public:
    DynamicLibraryView(DynamicLibraryWrapper dynamicLibraryWrapper, HMODULE handle)
        : dl{ dynamicLibraryWrapper }, handle{ handle }
    {
    }

    [[nodiscard]] void* getFunctionAddress(const char* functionName) const noexcept
    {
        return dl.GetProcAddress(handle, functionName);
    }

    [[nodiscard]] HMODULE getHandle() const noexcept
    {
        return handle;
    }

private:
    [[no_unique_address]] DynamicLibraryWrapper dl;
    HMODULE handle;
};

}
