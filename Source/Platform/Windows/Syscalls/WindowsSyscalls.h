#pragma once

#include <Windows.h>
#include <winternl.h>

#include "SyscallParams.h"
#include "WindowsSyscallIndex.h"

extern "C" std::size_t systemCall(const SyscallParams* params, ...);

struct IO_APC_ROUTINE;

struct WindowsSyscalls {
    static NTSTATUS NtCreateFile(
        HANDLE* fileHandle,
        ACCESS_MASK desiredAccess,
        OBJECT_ATTRIBUTES* objectAttributes,
        IO_STATUS_BLOCK* ioStatusBlock,
        LARGE_INTEGER* allocationSize,
        ULONG fileAttributes,
        ULONG shareAccess,
        ULONG createDisposition,
        ULONG createOptions,
        void* eaBuffer,
        ULONG eaLength) noexcept
    {
        const SyscallParams params{WindowsSyscallIndex::NtCreateFile, reinterpret_cast<std::uintptr_t>(fileHandle)};
        return static_cast<NTSTATUS>(systemCall(&params, desiredAccess, objectAttributes, ioStatusBlock, allocationSize, fileAttributes, shareAccess, createDisposition, createOptions, eaBuffer, eaLength));
    }

    static NTSTATUS NtReadFile(HANDLE fileHandle, HANDLE event, IO_APC_ROUTINE* apcRoutine, void* apcContext, IO_STATUS_BLOCK* ioStatusBlock, void* buffer, ULONG length, LARGE_INTEGER* byteOffset, ULONG* key) noexcept
    {
        const SyscallParams params{WindowsSyscallIndex::NtReadFile, reinterpret_cast<std::uintptr_t>(fileHandle)};
        return static_cast<NTSTATUS>(systemCall(&params, event, apcRoutine, apcContext, ioStatusBlock, buffer, length, byteOffset, key));
    }

    static NTSTATUS NtWriteFile(HANDLE fileHandle, HANDLE event, IO_APC_ROUTINE* apcRoutine, void* apcContext, IO_STATUS_BLOCK* ioStatusBlock, void* buffer, ULONG length, LARGE_INTEGER* byteOffset, ULONG* key) noexcept
    {
        const SyscallParams params{WindowsSyscallIndex::NtWriteFile, reinterpret_cast<std::uintptr_t>(fileHandle)};
        return static_cast<NTSTATUS>(systemCall(&params, event, apcRoutine, apcContext, ioStatusBlock, buffer, length, byteOffset, key));
    }

    static NTSTATUS NtSetInformationFile(HANDLE fileHandle, IO_STATUS_BLOCK* ioStatusBlock, void* fileInformation, ULONG length, FILE_INFORMATION_CLASS fileInformationClass) noexcept
    {
        const SyscallParams params{WindowsSyscallIndex::NtSetInformationFile, reinterpret_cast<std::uintptr_t>(fileHandle)};
        return static_cast<NTSTATUS>(systemCall(&params, ioStatusBlock, fileInformation, length, fileInformationClass));
    }

    static NTSTATUS NtClose(HANDLE handle) noexcept
    {
        const SyscallParams params{WindowsSyscallIndex::NtClose, reinterpret_cast<std::uintptr_t>(handle)};
        return static_cast<NTSTATUS>(systemCall(&params));
    }
};
