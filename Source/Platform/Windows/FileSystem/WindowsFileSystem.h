#pragma once

#include <cstddef>
#include <span>
#include <Windows.h>
#include <winternl.h>

#include <Platform/Windows/Syscalls/WindowsSyscalls.h>

struct WindowsFileSystem {
    static HANDLE openFileForReading(UNICODE_STRING fileName) noexcept
    {
        HANDLE handle;
        IO_STATUS_BLOCK statusBlock{};
        OBJECT_ATTRIBUTES objectAttributes{
            .Length{sizeof(OBJECT_ATTRIBUTES)},
            .RootDirectory{nullptr},
            .ObjectName{&fileName},
            .Attributes{OBJ_CASE_INSENSITIVE},
            .SecurityDescriptor{nullptr},
            .SecurityQualityOfService{nullptr}
        };
        if (NT_SUCCESS(WindowsSyscalls::NtCreateFile(&handle, FILE_GENERIC_READ, &objectAttributes, &statusBlock, nullptr, FILE_ATTRIBUTE_NORMAL, FILE_SHARE_READ | FILE_SHARE_WRITE | FILE_SHARE_DELETE, FILE_OPEN, FILE_SYNCHRONOUS_IO_NONALERT, nullptr, 0)))
            return handle;
        return HANDLE{};
    }

    static std::size_t readFile(HANDLE fileHandle, std::size_t fileOffset, std::span<std::byte> buffer) noexcept
    {
        IO_STATUS_BLOCK statusBlock{};
        LARGE_INTEGER offset{.QuadPart{static_cast<LONGLONG>(fileOffset)}};
        if (NT_SUCCESS(WindowsSyscalls::NtReadFile(fileHandle, nullptr, nullptr, nullptr, &statusBlock, buffer.data(), static_cast<ULONG>(buffer.size()), &offset, nullptr)) && statusBlock.Information <= buffer.size()) {
            return statusBlock.Information;
        }
        return 0;
    }
};
