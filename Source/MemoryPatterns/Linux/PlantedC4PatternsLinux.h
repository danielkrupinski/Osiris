#pragma once

#include <MemoryPatterns/PlantedC4Patterns.h>
#include <MemorySearch/BytePatternLiteral.h>

inline BombSiteOffset PlantedC4Patterns::m_nBombSite() const noexcept
{
    return clientPatternFinder("83 BF ? ? ? ? 01 0F 94 C0 C3"_pat).add(2).readOffset<BombSiteOffset>();
}

inline BombTickingOffset PlantedC4Patterns::m_bBombTicking() const noexcept
{
    return clientPatternFinder("74 EB 41 80 BC 24 ? ? ? ? ? 0F"_pat).add(6).readOffset<BombTickingOffset>();
}

inline BombBlowTimeOffset PlantedC4Patterns::m_flC4Blow() const noexcept
{
    return clientPatternFinder("F3 0F 10 83 ? ? ? ? F3 0F 5C C1 0F 2F E0"_pat).add(4).readOffset<BombBlowTimeOffset>();
}

inline BombDefuserOffset PlantedC4Patterns::m_hBombDefuser() const noexcept
{
    return clientPatternFinder("E8 ? ? ? ? 8B 8B ? ? ? ? 83 F9 FF 0F 84 ? ? ? ? 48 8B 3D"_pat).add(7).readOffset<BombDefuserOffset>();
}

inline BombDefuseEndTimeOffset PlantedC4Patterns::m_flDefuseCountDown() const noexcept
{
    return clientPatternFinder("74 ? F3 0F 10 80 ? ? ? ? 84"_pat).add(6).readOffset<BombDefuseEndTimeOffset>();
}
