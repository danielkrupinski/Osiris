#pragma once

#include <MemoryPatterns/PlantedC4Patterns.h>
#include <MemorySearch/BytePatternLiteral.h>

inline BombSiteOffset PlantedC4Patterns::m_nBombSite() const noexcept
{
    return BombSiteOffset{
        clientPatternFinder("83 B9 ? ? ? ? 01 0F 94 C0 C3"_pat).add(2).as<std::int32_t*>()
    };
}

inline BombTickingOffset PlantedC4Patterns::m_bBombTicking() const noexcept
{
    return BombTickingOffset{
        clientPatternFinder("74 ? 44 38 AE ? ? ? ? 74 ? 49"_pat).add(5).as<std::int32_t*>()
    };
}

inline BombBlowTimeOffset PlantedC4Patterns::m_flC4Blow() const noexcept
{
    return BombBlowTimeOffset{
        clientPatternFinder("F3 0F 10 ? ? ? ? ? BB FF"_pat).add(4).as<std::int32_t*>()
    };
}

inline BombDefuserOffset PlantedC4Patterns::m_hBombDefuser() const noexcept
{
    return BombDefuserOffset{
        clientPatternFinder("E8 ? ? ? ? 8B ? ? ? ? ? 45 33 ? 4C 8B ? ? ? ? ? 83"_pat).add(7).as<std::int32_t*>()
    };
}

inline BombDefuseEndTimeOffset PlantedC4Patterns::m_flDefuseCountDown() const noexcept
{
    return BombDefuseEndTimeOffset{
        clientPatternFinder("74 11 F3 0F 10 B7 ? ? ? ? 48"_pat).add(6).as<std::int32_t*>()
    };
}
