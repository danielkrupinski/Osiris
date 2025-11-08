#pragma once

#include <string_view>
#include <Platform/Macros/PlatformSpecific.h>

namespace build
{

constexpr auto MEMORY_CAPACITY = 1'000'000;

constexpr std::basic_string_view kOsirisDirectoryName{WIN64_LINUX(L"OsirisCS2", "OsirisCS2")};
constexpr std::basic_string_view kConfigDirectoryName{WIN64_LINUX(L"configs", "configs")};
constexpr auto kConfigFileBufferSize{4096};

}
