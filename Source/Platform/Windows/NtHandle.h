#pragma once

#include <memory>
#include <Windows.h>

#include "Syscalls/WindowsSyscalls.h"

struct NtHandleDeleter {
    void operator()(HANDLE handle) const noexcept
    {
        if (handle)
            WindowsSyscalls::NtClose(handle);
    }
};

using NtHandle = std::unique_ptr<void, NtHandleDeleter>;
