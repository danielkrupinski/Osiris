#pragma once

#include <cstdint>

#include <Platform/Windows/Syscalls/WindowsSyscalls.h>

struct WindowsMemory {
    [[nodiscard]] static bool isMemoryReadable(std::uintptr_t address) noexcept
    {
        MEMORY_BASIC_INFORMATION mbi;
        const auto status = WindowsSyscalls::NtQueryVirtualMemory(
            reinterpret_cast<HANDLE>(-1),
            reinterpret_cast<void*>(address),
            MEMORY_INFORMATION_CLASS::MemoryBasicInformation,
            &mbi,
            sizeof(mbi),
            nullptr
        );

        constexpr auto kReadableMask = PAGE_READONLY | PAGE_READWRITE | PAGE_WRITECOPY | PAGE_EXECUTE_READ | PAGE_EXECUTE_READWRITE | PAGE_EXECUTE_WRITECOPY;
        constexpr auto kUnreadableMask = PAGE_NOACCESS | PAGE_GUARD;

        return NT_SUCCESS(status) && mbi.State == MEM_COMMIT && (mbi.Protect & kReadableMask) && !(mbi.Protect & kUnreadableMask);
    }
};
