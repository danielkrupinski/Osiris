#pragma once

#include <dlfcn.h>

struct link_map;

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

    [[nodiscard]] link_map* getLinkMap() const noexcept
    {
        link_map* map = nullptr;
        dl.dlinfo(handle, RTLD_DI_LINKMAP, &map);
        return map;
    }

private:
    [[no_unique_address]] DynamicLibraryWrapper dl;
    void* handle;
};

}
