#pragma once

#include <Platform/Macros/PlatformSpecific.h>
#include <Utils/Pad.h>
#include <CSGO/UtlMap.h>

namespace csgo
{

struct PaintKit;
struct StickerKit;
struct EconItemQualityDefinition;
struct AlternateIconData;
struct EconMusicDefinition;
struct EconItemDefinitionPOD;

struct ItemSchemaPOD {
    PAD(WIN32_LINUX(0x88, 0xB8));
    UtlMap<int, EconItemQualityDefinition> qualities;
    PAD(WIN32_LINUX(0x48, 0x60));
    UtlMap<int, EconItemDefinitionPOD*> itemsSorted;
    PAD(WIN32_LINUX(0x60, 0x88));
    UtlMap<int, const char*> revolvingLootLists;
    PAD(WIN32_LINUX(0x80, 0xB0));
    UtlMap<std::uint64_t, AlternateIconData> alternateIcons;
    PAD(WIN32_LINUX(0x48, 0x60));
    UtlMap<int, PaintKit*> paintKits;
    UtlMap<int, StickerKit*> stickerKits;
    PAD(WIN32_LINUX(0x11C, 0x1A0));
    UtlMap<int, EconMusicDefinition*> musicKits;
};

}
