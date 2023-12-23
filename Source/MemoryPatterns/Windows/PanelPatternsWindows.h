#pragma once

#include <MemoryPatterns/PanelPatterns.h>
#include <MemorySearch/BytePatternLiteral.h>

inline cs2::CPanel2D::Create* PanelPatterns::create() const noexcept
{
    return clientPatternFinder("48 89 5C 24 ? 48 89 74 24 ? 57 48 83 EC 30 48 8B F1 48 8B FA B9 ? ? ? ? E8 ? ? ? ? 48 8B D8 48 85 C0 74 5C"_pat).as<cs2::CPanel2D::Create*>();
}
