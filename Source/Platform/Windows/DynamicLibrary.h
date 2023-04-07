#pragma once

#include <Windows.h>

#include "PebLdr.h"
#include "PortableExecutable.h"

namespace windows_platform
{

template <typename PlatformApi>
class DynamicLibrary {
public:
    DynamicLibrary(PlatformApi platformApi, const char* libraryName)
        : platformApi{ platformApi }, handle{ PebLdr{ platformApi.getPeb()->ldr }.getModuleHandle(libraryName) }
    {
    }

    [[nodiscard]] const void* getFunctionAddress(const char* functionName) const noexcept
    {
        return PortableExecutable{ reinterpret_cast<const std::byte*>(handle) }.getExport(functionName);
    }

    [[nodiscard]] std::span<const std::byte> getCodeSection() const noexcept
    {
        return PortableExecutable{ reinterpret_cast<const std::byte*>(handle) }.getCodeSection();
    }

    [[nodiscard]] HMODULE getHandle() const noexcept
    {
        return handle;
    }

private:
    [[no_unique_address]] PlatformApi platformApi;
    HMODULE handle;
};

}
