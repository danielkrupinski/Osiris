#pragma once

#include <cstddef>
#include <span>

#include "FileDescriptor.h"
#include <Platform/Linux/LinuxPlatformApi.h>

struct LinuxInputFile {
    explicit LinuxInputFile(const char* path) noexcept
        : fileDescriptor{LinuxPlatformApi::open(path, O_RDONLY)}
    {
    }

    [[nodiscard]] std::size_t read(std::size_t fileOffset, std::span<std::byte> buffer) const noexcept
    {
        const auto readBytes{LinuxPlatformApi::pread(fileDescriptor.get(), buffer.data(), buffer.size(), fileOffset)};
        if (readBytes > 0)
            return readBytes;
        return 0;
    }

private:
    FileDescriptor fileDescriptor;
};
