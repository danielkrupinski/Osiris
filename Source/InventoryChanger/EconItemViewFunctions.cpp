#include "EconItemViewFunctions.h"
#include <MemorySearch/BytePatternLiteral.h>
#include <Platform/Macros/IsPlatform.h>

[[nodiscard]] EconItemViewFunctions createEconItemViewFunctions(const PatternFinder& clientPatternFinder)
{
    return EconItemViewFunctions{
#if IS_WIN32()
        .clearInventoryImageRGBA = clientPatternFinder("55 8B EC 81 EC ? ? ? ? 57 8B F9 C7 47"_pat).as<csgo::ClearInventoryImageRGBA>(),
        .getSOCData = clientPatternFinder("E8 ? ? ? ? 32 C9"_pat).add(1).relativeToAbsolute().as<csgo::GetSOCData>()
#elif IS_LINUX()
        .clearInventoryImageRGBA = clientPatternFinder("E8 ? ? ? ? 83 C3 01 49 83 C4 08 41 39 5D 50"_pat).add(1).relativeToAbsolute().as<csgo::ClearInventoryImageRGBA>(),
        .getSOCData = clientPatternFinder("E8 ? ? ? ? 4D 85 E4 0F 94 C1"_pat).add(1).relativeToAbsolute().as<csgo::GetSOCData>()
#endif
    };
}
