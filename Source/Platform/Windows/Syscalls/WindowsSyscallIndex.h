#pragma once

#include <cstddef>

namespace win
{

enum class SyscallIndexWindows10and11 : std::size_t {
    NtReadFile = 6,
    NtWriteFile = 8,
    NtClose = 15,
    NtSetInformationFile = 39,
    NtCreateFile = 85
};

}

using WindowsSyscallIndex = win::SyscallIndexWindows10and11;
