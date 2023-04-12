#pragma once

#include <Windows.h>

#include "PebLdr.h"
#include "PortableExecutable.h"
#include <Utils/SafeAddress.h>

namespace windows_platform
{

template <typename PlatformApi>
class DynamicLibrary {
public:
    explicit DynamicLibrary(const char* libraryName)
        : handle{ PebLdr{ PlatformApi::getPeb()->ldr }.getModuleHandle(libraryName) }
    {
    }

    [[nodiscard]] SafeAddress getFunctionAddress(const char* functionName) const noexcept
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
    HMODULE handle;
};

}
