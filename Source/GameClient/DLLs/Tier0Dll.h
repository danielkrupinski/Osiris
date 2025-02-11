#pragma once

#include <CS2/Classes/CUtlFilenameSymbolTable.h>
#include <CS2/Constants/DllNames.h>
#include <Platform/DynamicLibrary.h>
#include <Platform/Macros/IsPlatform.h>

namespace cs2
{
    struct IMemAlloc;
}

struct Tier0Dll : DynamicLibrary {
    Tier0Dll() noexcept
        : DynamicLibrary{ cs2::TIER0_DLL }
    {
    }

    [[nodiscard]] cs2::CUtlFilenameSymbolTable::String* filenameSymbolTableString() const noexcept
    {
#if IS_WIN64()
        return getFunctionAddress("?String@CUtlFilenameSymbolTable@@QEAA_NAEBIPEADH@Z").as<cs2::CUtlFilenameSymbolTable::String*>();
#elif IS_LINUX()
        return getFunctionAddress("_ZN23CUtlFilenameSymbolTable6StringERKjPci").as<cs2::CUtlFilenameSymbolTable::String*>();
#endif
    }

    [[nodiscard]] cs2::IMemAlloc** memAlloc() const noexcept
    {
        return getFunctionAddress("g_pMemAlloc").as<cs2::IMemAlloc**>();
    }
};
