#pragma once

#include <cstddef>
#include <span>
#include <Windows.h>
#include <winternl.h>

#include <Platform/Windows/NtHandle.h>
#include "WindowsFileSystem.h"

struct WindowsInputFile {
    explicit WindowsInputFile(UNICODE_STRING fileName) noexcept
        : handle{WindowsFileSystem::openFileForReading(fileName)}
    {
    }

    [[nodiscard]] std::size_t read(std::size_t fileOffset, std::span<std::byte> buffer) const noexcept
    {
        return WindowsFileSystem::readFile(handle.get(), fileOffset, buffer);
    }

private:
    NtHandle handle;
};
