#pragma once

#include <dlfcn.h>

#include "DynamicLibraryView.h"

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

    [[nodiscard]] bool isValid() const noexcept
    {
        return handle != nullptr;
    }

    [[nodiscard]] DynamicLibraryView<DynamicLibraryWrapper> getView() const noexcept
    {
        return { dl, handle };
    }

    ~SharedObject() noexcept
    {
        if (handle != nullptr)
            dl.dlclose(handle);
    }

private:
    [[no_unique_address]] DynamicLibraryWrapper dl;
    void* handle = nullptr;
};

}
