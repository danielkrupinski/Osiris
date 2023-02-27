#include <CSGO/Functions.h>
#include "EconItemFunctions.h"
#include <Platform/Macros/IsPlatform.h>

[[nodiscard]] EconItemFunctions createEconItemFunctions(const PatternFinder& clientPatternFinder)
{
    return EconItemFunctions{
#if IS_WIN32()
        .setDynamicAttributeValue = clientPatternFinder("55 8B EC 83 E4 F8 83 EC 3C 53 8B 5D 08 56 57 6A"_pat).as<csgo::SetDynamicAttributeValue>(),
        .removeDynamicAttribute = clientPatternFinder("55 8B EC 83 EC 08 8B C1 89 45 F8"_pat).as<csgo::RemoveDynamicAttribute>(),
        .setCustomName = clientPatternFinder("E8 ? ? ? ? 8B 46 78 C1 E8 0A A8 01 74 13 8B 46 34"_pat).add(1).relativeToAbsolute().as<csgo::SetCustomName>()
#elif IS_LINUX()
        .setDynamicAttributeValue = clientPatternFinder("48 8B 7A 18 48 85 FF 0F 84 ? ? ? ? 55 48 89 E5 41 57 41 56 49 89 D6"_pat).as<csgo::SetDynamicAttributeValue>(),
        .removeDynamicAttribute = clientPatternFinder("E8 ? ? ? ? 80 3D ? ? ? ? ? 75 14 48 8D 3D ? ? ? ? E8 ? ? ? ? 85 C0 0F 85 ? ? ? ? C7 45"_pat).add(1).relativeToAbsolute().as<csgo::RemoveDynamicAttribute>(),
        .setCustomName = clientPatternFinder("E8 ? ? ? ? 8B 83 ? ? ? ? E9 ? ? ? ? 48 8B 05 ? ? ? ? 44 8B A0"_pat).add(1).relativeToAbsolute().as<csgo::SetCustomName>()
#endif
    };
}
