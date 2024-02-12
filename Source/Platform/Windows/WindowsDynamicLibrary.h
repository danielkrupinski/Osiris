#pragma once

#include <Windows.h>

#include "PebLdr.h"
#include "PortableExecutable.h"
#include <Utils/GenericPointer.h>
#include <Utils/MemorySection.h>
#include "WindowsPlatformApi.h"
#include "WindowsVmtFinderParams.h"

class WindowsDynamicLibrary {
public:
    explicit WindowsDynamicLibrary(const char* libraryName)
        : handle{ PebLdr{ WindowsPlatformApi::getPeb()->ldr }.getModuleHandle(libraryName) }
    {
    }

    [[nodiscard]] explicit operator bool() const noexcept
    {
        return handle != nullptr;
    }

    [[nodiscard]] GenericPointer getFunctionAddress(const char* functionName) const noexcept
    {
        if (handle)
            return portableExecutable().getExport(functionName);
        return {};
    }

    [[nodiscard]] MemorySection getCodeSection() const noexcept
    {
        if (handle)
            return portableExecutable().getCodeSection();
        return {};
    }

    [[nodiscard]] MemorySection getVmtSection() const noexcept
    {
        if (handle)
            return portableExecutable().getVmtSection();
        return {};
    }

    [[nodiscard]] MemorySection getDataSection() const noexcept
    {
        if (handle)
            return portableExecutable().getDataSection();
        return {};
    }

    [[nodiscard]] WindowsVmtFinderParams getVmtFinderParams() const noexcept
    {
        return {getDataSection(), getVmtSection(), handle};
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
