#pragma once

#include <cstddef>
#include <span>

#include "PortableExecutable.h"
#include "DynamicLibrary.h"

namespace windows_platform
{

template <typename PlatformApi>
[[nodiscard]] std::span<const std::byte> getCodeSection(DynamicLibrary<PlatformApi> library)
{
    return PortableExecutable{ reinterpret_cast<const std::byte*>(library.getHandle()) }.getCodeSection();
}

}
