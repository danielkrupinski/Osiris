#pragma once

#include <dlfcn.h>

#include "DynamicLibraryView.h"

namespace linux_platform
{

template <typename PlatformApi>
class SharedObject {
public:
    SharedObject(PlatformApi platformApi, const char* libraryName)
        : platformApi{ platformApi }, handle{ getModuleHandle(libraryName) } {}

    [[nodiscard]] bool isValid() const noexcept
    {
        return handle != nullptr;
    }

    [[nodiscard]] DynamicLibraryView<PlatformApi> getView() const noexcept
    {
        return { platformApi, handle };
    }

private:
    [[nodiscard]] void* getModuleHandle(const char* libraryName)
    {
        const auto handle = platformApi.dlopen(libraryName, RTLD_LAZY | RTLD_NOLOAD);
        if (handle)
            platformApi.dlclose(handle);
        return handle;
    }

    [[no_unique_address]] PlatformApi platformApi;
    void* handle = nullptr;
};

}
