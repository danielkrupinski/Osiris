#pragma once

#include <cstdint>

#include <Helpers/PatternFinder.h>
#include <Platform/Macros/IsPlatform.h>
#include <CSGO/Functions.h>
#include <CSGO/Helpers/EconItemFunctions.h>
#include <MemorySearch/BytePatternLiteral.h>

[[nodiscard]] inline EconItemFunctions createEconItemFunctions(const helpers::PatternFinder& clientPatternFinder)
{
    return EconItemFunctions{
#if IS_WIN32()
        .setDynamicAttributeValue = clientPatternFinder("55 8B EC 83 E4 F8 83 EC 3C 53 8B 5D 08 56 57 6A"_pat).as<csgo::SetDynamicAttributeValue>(),
        .removeDynamicAttribute = clientPatternFinder("55 8B EC 83 EC 08 8B C1 89 45 F8"_pat).as<csgo::RemoveDynamicAttribute>(),
        .setCustomName = clientPatternFinder("E8 ? ? ? ? 8B 46 78 C1 E8 0A A8 01 74 13 8B 46 34"_pat).add(1).relativeToAbsolute().as<csgo::SetCustomName>()
#elif IS_LINUX()
        .setDynamicAttributeValue = clientPatternFinder("41 8B 06 49 8D 7D 08"_pat).add(-95).as<csgo::SetDynamicAttributeValue>(),
        .removeDynamicAttribute = clientPatternFinder("E8 ? ? ? ? 80 3D ? ? ? ? ? 75 14 48 8D 3D ? ? ? ? E8 ? ? ? ? 85 C0 0F 85 ? ? ? ? C7 45"_pat).add(1).relativeToAbsolute().as<csgo::RemoveDynamicAttribute>(),
        .setCustomName = clientPatternFinder("E8 ? ? ? ? 41 8B 84 24 ? ? ? ? E9 ? ? ? ? 8B 98"_pat).add(1).relativeToAbsolute().as<csgo::SetCustomName>()
#endif
    };
}
