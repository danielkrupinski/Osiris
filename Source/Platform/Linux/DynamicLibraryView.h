#pragma once

namespace linux_platform
{

template <typename DynamicLibraryWrapper>
class DynamicLibraryView {
public:
    DynamicLibraryView(DynamicLibraryWrapper dynamicLibraryWrapper, void* handle)
        : dl{ dynamicLibraryWrapper }, handle{ handle }
    {
    }

    [[nodiscard]] void* getFunctionAddress(const char* functionName) const noexcept
    {
        return dl.dlsym(handle, functionName);
    }

private:
    [[no_unique_address]] DynamicLibraryWrapper dl;
    void* handle;
};

}
