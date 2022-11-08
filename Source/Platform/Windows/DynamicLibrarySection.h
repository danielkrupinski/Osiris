#pragma once

#include <cstddef>
#include <span>

#include <Psapi.h>
#include <Windows.h>

#include "DynamicLibraryView.h"

namespace windows_platform
{

template <typename DynamicLibraryWrapper>
[[nodiscard]] std::span<const std::byte> getCodeSection(DynamicLibraryView<DynamicLibraryWrapper> library)
{
    // TODO: this returns whole module, return only .text section
    if (MODULEINFO moduleInfo; GetModuleInformation(GetCurrentProcess(), library.getHandle(), &moduleInfo, sizeof(moduleInfo)))
        return { reinterpret_cast<const std::byte*>(moduleInfo.lpBaseOfDll), moduleInfo.SizeOfImage };
    return {};
}

}
