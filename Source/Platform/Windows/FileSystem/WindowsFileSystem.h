#pragma once

#include <cstddef>
#include <span>
#include <Windows.h>
#include <winternl.h>

#include <Platform/Windows/Syscalls/WindowsSyscalls.h>
#include <Utils/Wcslen.h>

struct WindowsFileSystem {
    static HANDLE openFileForReading(UNICODE_STRING fileName) noexcept
    {
        HANDLE handle;
        IO_STATUS_BLOCK statusBlock{};
        OBJECT_ATTRIBUTES objectAttributes{
            .Length = sizeof(OBJECT_ATTRIBUTES),
            .RootDirectory = nullptr,
            .ObjectName = &fileName,
            .Attributes = OBJ_CASE_INSENSITIVE,
            .SecurityDescriptor = nullptr,
            .SecurityQualityOfService = nullptr
        };
        if (NT_SUCCESS(WindowsSyscalls::NtCreateFile(&handle, FILE_GENERIC_READ, &objectAttributes, &statusBlock, nullptr, FILE_ATTRIBUTE_NORMAL, FILE_SHARE_READ | FILE_SHARE_WRITE | FILE_SHARE_DELETE, FILE_OPEN, FILE_SYNCHRONOUS_IO_NONALERT, nullptr, 0)))
            return handle;
        return INVALID_HANDLE_VALUE;
    }

    static HANDLE createFileForOverwrite(const wchar_t* ntPath) noexcept
    {
        HANDLE handle;
        IO_STATUS_BLOCK statusBlock{};
        const auto pathLength = utils::wcslen(ntPath);
        UNICODE_STRING fileName{
            .Length = static_cast<USHORT>(pathLength * sizeof(wchar_t)),
            .MaximumLength = static_cast<USHORT>(pathLength * sizeof(wchar_t)),
            .Buffer = const_cast<wchar_t*>(ntPath)
        };
        OBJECT_ATTRIBUTES objectAttributes{
            .Length = sizeof(OBJECT_ATTRIBUTES),
            .RootDirectory = nullptr,
            .ObjectName = &fileName,
            .Attributes = OBJ_CASE_INSENSITIVE,
            .SecurityDescriptor = nullptr,
            .SecurityQualityOfService = nullptr
        };
        if (NT_SUCCESS(WindowsSyscalls::NtCreateFile(&handle, FILE_GENERIC_WRITE | DELETE, &objectAttributes, &statusBlock, nullptr, FILE_ATTRIBUTE_NORMAL, FILE_SHARE_READ, FILE_SUPERSEDE, FILE_NON_DIRECTORY_FILE | FILE_SYNCHRONOUS_IO_NONALERT, nullptr, 0)))
            return handle;
        return INVALID_HANDLE_VALUE;
    }

    static void createDirectory(const wchar_t* ntPath) noexcept
    {
        IO_STATUS_BLOCK statusBlock{};
        const auto pathLength = utils::wcslen(ntPath);
        UNICODE_STRING directoryName{
            .Length = static_cast<USHORT>(pathLength * sizeof(wchar_t)),
            .MaximumLength = static_cast<USHORT>(pathLength * sizeof(wchar_t)),
            .Buffer = const_cast<wchar_t*>(ntPath)
        };
        OBJECT_ATTRIBUTES objectAttributes{
            .Length = sizeof(OBJECT_ATTRIBUTES),
            .RootDirectory = nullptr,
            .ObjectName = &directoryName,
            .Attributes = OBJ_CASE_INSENSITIVE,
            .SecurityDescriptor = nullptr,
            .SecurityQualityOfService = nullptr
        };

        if (HANDLE handle; NT_SUCCESS(WindowsSyscalls::NtCreateFile(&handle, FILE_LIST_DIRECTORY | SYNCHRONIZE, &objectAttributes, &statusBlock, nullptr, FILE_ATTRIBUTE_NORMAL, FILE_SHARE_READ | FILE_SHARE_WRITE, FILE_CREATE, FILE_DIRECTORY_FILE | FILE_SYNCHRONOUS_IO_NONALERT | FILE_OPEN_FOR_BACKUP_INTENT, nullptr, 0)))
            WindowsSyscalls::NtClose(handle);
    }

    static std::size_t readFile(HANDLE fileHandle, std::size_t fileOffset, void* buffer, std::size_t bufferSize) noexcept
    {
        IO_STATUS_BLOCK statusBlock{};
        LARGE_INTEGER offset{.QuadPart = static_cast<LONGLONG>(fileOffset)};
        if (NT_SUCCESS(WindowsSyscalls::NtReadFile(fileHandle, nullptr, nullptr, nullptr, &statusBlock, buffer, static_cast<ULONG>(bufferSize), &offset, nullptr)) && statusBlock.Information <= bufferSize) {
            return statusBlock.Information;
        }
        return 0;
    }

    static std::size_t writeFile(HANDLE fileHandle, std::size_t fileOffset, void* buffer, std::size_t bufferSize) noexcept
    {
        IO_STATUS_BLOCK statusBlock{};
        LARGE_INTEGER offset{.QuadPart = static_cast<LONGLONG>(fileOffset)};
        if (NT_SUCCESS(WindowsSyscalls::NtWriteFile(fileHandle, nullptr, nullptr, nullptr, &statusBlock, buffer, static_cast<ULONG>(bufferSize), &offset, nullptr)) && statusBlock.Information <= bufferSize) {
            return statusBlock.Information;
        }
        return 0;
    }

    static void renameFile(HANDLE fileHandle, const wchar_t* newFilePath) noexcept
    {
        IO_STATUS_BLOCK statusBlock{};

        const auto pathLength = utils::wcslen(newFilePath);

        constexpr auto kMaxPathLength{200};
        if (pathLength > kMaxPathLength) {
            assert(false);
            return;
        }

        alignas(FILE_RENAME_INFO) std::byte buffer[sizeof(FILE_RENAME_INFO) + kMaxPathLength * sizeof(wchar_t)];

        FILE_RENAME_INFO* info = new (&buffer) FILE_RENAME_INFO{};
        info->ReplaceIfExists = TRUE;
        info->FileNameLength = static_cast<DWORD>(pathLength * sizeof(wchar_t));
        std::memcpy(&info->FileName, newFilePath, pathLength * sizeof(wchar_t));
        constexpr wchar_t kNullTerminator{L'\0'};
        std::memcpy(reinterpret_cast<std::byte*>(&info->FileName) + pathLength * sizeof(wchar_t), &kNullTerminator, sizeof(kNullTerminator));

        constexpr std::underlying_type_t<FILE_INFORMATION_CLASS> fileRenameInformationClass{10};
        FILE_INFORMATION_CLASS fileRenameInformationClassEnum{};
        std::memcpy(&fileRenameInformationClassEnum, &fileRenameInformationClass, sizeof(fileRenameInformationClass));
        WindowsSyscalls::NtSetInformationFile(fileHandle, &statusBlock, info, sizeof(FILE_RENAME_INFO) + kMaxPathLength * sizeof(wchar_t), fileRenameInformationClassEnum);
    }
};
