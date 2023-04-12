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
        if (handle)
            return portableExecutable().getExport(functionName);
        return SafeAddress{ 0 };
    }

    [[nodiscard]] std::span<const std::byte> getCodeSection() const noexcept
    {
        if (handle)
            return portableExecutable().getCodeSection();
        return {};
    }

    [[nodiscard]] HMODULE getHandle() const noexcept
    {
        return handle;
    }

private:
    [[nodiscard]] PortableExecutable portableExecutable() const noexcept
    {
        return PortableExecutable{ reinterpret_cast<const std::byte*>(handle) };
    }

    HMODULE handle;
};

}
