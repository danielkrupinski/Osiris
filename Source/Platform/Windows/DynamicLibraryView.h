#pragma once

namespace windows_platform
{

template <typename DynamicLibraryWrapper>
class DynamicLibraryView {
public:
    DynamicLibraryView(DynamicLibraryWrapper dynamicLibraryWrapper, const char* libraryName)
        : dl{ dynamicLibraryWrapper }, handle{ dl.GetModuleHandleA(libraryName) }
    {
    }

    [[nodiscard]] void* getFunctionAddress(const char* functionName) const noexcept
    {
        return dl.GetProcAddress(handle, functionName);
    }

private:
    [[no_unique_address]] DynamicLibraryWrapper dl;
    HMODULE handle;
};

}
