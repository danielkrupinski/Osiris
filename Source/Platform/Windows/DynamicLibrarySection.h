#pragma once

#include <cstddef>
#include <span>

#include "PortableExecutable.h"
#include "DynamicLibraryView.h"

namespace windows_platform
{

template <typename DynamicLibraryWrapper>
[[nodiscard]] std::span<const std::byte> getCodeSection(DynamicLibraryView<DynamicLibraryWrapper> library)
{
    return PortableExecutable{ reinterpret_cast<const std::byte*>(library.getHandle()) }.getCodeSection();
}

}
