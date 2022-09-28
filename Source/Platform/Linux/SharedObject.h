#pragma once

#include <dlfcn.h>

namespace linux_platform
{

template <typename DynamicLibraryWrapper>
class SharedObject {
public:
    SharedObject(DynamicLibraryWrapper dynamicLibraryWrapper, const char* libraryName)
        : dl{ dynamicLibraryWrapper }, handle{ dl.dlopen(libraryName, RTLD_LAZY | RTLD_NOLOAD) } {}

    SharedObject(const SharedObject&) = delete;
    SharedObject& operator=(const SharedObject&) = delete;
    SharedObject(SharedObject&&) = delete;
    SharedObject& operator=(SharedObject&&) = delete;

    [[nodiscard]] void* getFunctionAddress(const char* functionName) const noexcept
    {
        return dl.dlsym(handle, functionName);
    }

    [[nodiscard]] bool isValid() const noexcept
    {
        return handle != nullptr;
    }

    ~SharedObject() noexcept
    {
        dl.dlclose(handle);
    }

private:
    [[no_unique_address]] DynamicLibraryWrapper dl;
    void* handle = nullptr;
};

}
