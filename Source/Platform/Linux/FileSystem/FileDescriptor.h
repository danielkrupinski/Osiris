#pragma once

#include <Platform/Linux/LinuxPlatformApi.h>

struct FileDescriptor {
    explicit FileDescriptor(int fileDescriptor) noexcept
        : fileDescriptor{fileDescriptor}
    {
    }

    FileDescriptor(const FileDescriptor&) = delete;
    FileDescriptor(FileDescriptor&&) = delete;
    FileDescriptor& operator=(const FileDescriptor&) = delete;
    FileDescriptor& operator=(FileDescriptor&&) = delete;

    [[nodiscard]] int get() const noexcept
    {
        return fileDescriptor;
    }

    [[nodiscard]] bool isValid() const noexcept
    {
        return fileDescriptor >= 0;
    }

    ~FileDescriptor() noexcept
    {
        if (isValid())
            LinuxPlatformApi::close(fileDescriptor);
    }

private:
    int fileDescriptor;
};
