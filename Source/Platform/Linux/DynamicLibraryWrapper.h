#pragma once

#include <dlfcn.h>

namespace linux_platform
{

struct DynamicLibraryWrapper {
    void* dlopen(const char* file, int mode) const noexcept
    {
        return ::dlopen(file, mode);
    }

    void* dlsym(void* handle, const char* name) const noexcept
    {
        return ::dlsym(handle, name);
    }

    int dlclose(void* handle) const noexcept
    {
        return ::dlclose(handle);
    }
};

}
