#pragma once

#include <Windows.h>

#include "DynamicLibraryView.h"
#include "PebLdr.h"

namespace windows_platform
{

template <typename PlatformApi>
class DynamicLibrary {
public:
    DynamicLibrary(PlatformApi platformApi, const char* libraryName)
        : platformApi{ platformApi }, handle{ PebLdr{ platformApi.getPeb()->ldr }.getModuleHandle(libraryName) }
    {
    }

    [[nodiscard]] DynamicLibraryView<PlatformApi> getView() const noexcept
    {
        return { platformApi, handle };
    }

private:
    [[no_unique_address]] PlatformApi platformApi;
    HMODULE handle;
};

}
