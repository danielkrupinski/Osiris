#pragma once

#include <MemoryPatterns/PlantedC4Patterns.h>
#include <MemorySearch/BytePatternLiteral.h>

inline BombSiteOffset PlantedC4Patterns::m_nBombSite() const noexcept
{
    return BombSiteOffset{
        clientPatternFinder("83 BF ? ? ? ? 01 0F 94 C0 C3"_pat).add(2).as<std::int32_t*>()
    };
}

inline BombTickingOffset PlantedC4Patterns::m_bBombTicking() const noexcept
{
    return BombTickingOffset{
        clientPatternFinder("74 EB 41 80 BC 24 ? ? ? ? ? 0F"_pat).add(6).as<std::int32_t*>()
    };
}

inline BombBlowTimeOffset PlantedC4Patterns::m_flC4Blow() const noexcept
{
    return BombBlowTimeOffset{
        clientPatternFinder("F3 0F 10 83 ? ? ? ? F3 0F 5C C1 0F 2F E0"_pat).add(4).as<std::int32_t*>()
    };
}

inline BombDefuserOffset PlantedC4Patterns::m_hBombDefuser() const noexcept
{
    return BombDefuserOffset{
        clientPatternFinder("E8 ? ? ? ? 8B 8B ? ? ? ? 83 F9 FF 0F 84 ? ? ? ? 48 8B 3D"_pat).add(7).as<std::int32_t*>()
    };
}

inline BombDefuseEndTimeOffset PlantedC4Patterns::m_flDefuseCountDown() const noexcept
{
    return BombDefuseEndTimeOffset{
        clientPatternFinder("74 ? F3 0F 10 80 ? ? ? ? 84"_pat).add(6).as<std::int32_t*>()
    };
}
