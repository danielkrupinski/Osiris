#pragma once

#include <Windows.h>
#include "PortableExecutable.h"

namespace windows_platform
{

template <typename DynamicLibraryWrapper>
class DynamicLibraryView {
public:
    DynamicLibraryView(DynamicLibraryWrapper dynamicLibraryWrapper, HMODULE handle)
        : dl{ dynamicLibraryWrapper }, handle{ handle }
    {
    }

    [[nodiscard]] const void* getFunctionAddress(const char* functionName) const noexcept
    {
        return PortableExecutable{ reinterpret_cast<const std::byte*>(handle) }.getExport(functionName);
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
