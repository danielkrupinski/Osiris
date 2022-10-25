#pragma once

#include <Platform/PlatformSpecific.h>
#include <SDK/Pad.h>
#include <SDK/UtlMap.h>

struct EconItemQualityDefinition;
struct AlternateIconData;
struct PaintKit;
struct StickerKit;
struct EconMusicDefinition;
using EconItemDefinitionPointer = std::uintptr_t;

namespace csgo::pod
{

struct ItemSchema {
    PAD(WIN32_LINUX(0x88, 0xB8))
    UtlMap<int, EconItemQualityDefinition> qualities;
    PAD(WIN32_LINUX(0x48, 0x60))
    UtlMap<int, EconItemDefinitionPointer> itemsSorted;
    PAD(WIN32_LINUX(0x60, 0x88))
    UtlMap<int, const char*> revolvingLootLists;
    PAD(WIN32_LINUX(0x80, 0xB0))
    UtlMap<std::uint64_t, AlternateIconData> alternateIcons;
    PAD(WIN32_LINUX(0x48, 0x60))
    UtlMap<int, PaintKit*> paintKits;
    UtlMap<int, StickerKit*> stickerKits;
    PAD(WIN32_LINUX(0x11C, 0x1A0))
    UtlMap<int, EconMusicDefinition*> musicKits;
};

}
