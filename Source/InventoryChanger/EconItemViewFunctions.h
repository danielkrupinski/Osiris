#pragma once

#include <cstdint>

#include <Helpers/PatternFinder.h>
#include <MemorySearch/BytePatternLiteral.h>
#include <Platform/Macros/IsPlatform.h>
#include <CSGO/Helpers/EconItemViewFunctions.h>

[[nodiscard]] inline EconItemViewFunctions createEconItemViewFunctions(const helpers::PatternFinder& clientPatternFinder)
{
    return EconItemViewFunctions{
#if IS_WIN32()
        .clearInventoryImageRGBA = clientPatternFinder("55 8B EC 81 EC ? ? ? ? 57 8B F9 C7 47"_pat).as<csgo::ClearInventoryImageRGBA>(),
        .getSOCData = clientPatternFinder("E8 ? ? ? ? 32 C9"_pat).add(1).relativeToAbsolute().as<csgo::GetSOCData>()
#elif IS_LINUX()
        .clearInventoryImageRGBA = clientPatternFinder("E8 ? ? ? ? 83 C3 01 49 83 C4 08 41 3B 5D 50"_pat).add(1).relativeToAbsolute().as<csgo::ClearInventoryImageRGBA>(),
        .getSOCData = clientPatternFinder("E8 ? ? ? ? 5B 44 89 EE"_pat).add(1).relativeToAbsolute().as<csgo::GetSOCData>()
#endif
    };
}
