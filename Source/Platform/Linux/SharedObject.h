#pragma once

#include <dlfcn.h>

struct link_map;

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

    [[nodiscard]] void* getFunctionAddress(const char* functionName) const noexcept
    {
        return platformApi.dlsym(handle, functionName);
    }

    [[nodiscard]] link_map* getLinkMap() const noexcept
    {
        link_map* map = nullptr;
        platformApi.dlinfo(handle, RTLD_DI_LINKMAP, &map);
        return map;
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
