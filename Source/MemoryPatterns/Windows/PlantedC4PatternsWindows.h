#pragma once

#include <GlobalContext.h>
#include <MemoryPatterns/PlantedC4Patterns.h>
#include <MemorySearch/BytePatternLiteral.h>

inline BombSiteOffset PlantedC4Patterns::m_nBombSite() noexcept
{
    return BombSiteOffset{
        GlobalContext::instance().clientPatternFinder("01 75 ? 48 8B 0D ? ? ? ? 48 8B 09 E8 ? ? ? ? 84 C0 74"_pat).add(14).abs().add(2).as<std::int32_t*>()
    };
}

inline BombTickingOffset PlantedC4Patterns::m_bBombTicking() noexcept
{
    return BombTickingOffset{
        GlobalContext::instance().clientPatternFinder("74 ? 44 38 AE ? ? ? ? 74 ? 49"_pat).add(5).as<std::int32_t*>()
    };
}

inline BombBlowTimeOffset PlantedC4Patterns::m_flC4Blow() noexcept
{
    return BombBlowTimeOffset{
        GlobalContext::instance().clientPatternFinder("F3 0F 10 ? ? ? ? ? BB FF"_pat).add(4).as<std::int32_t*>()
    };
}

inline BombDefuserOffset PlantedC4Patterns::m_hBombDefuser() noexcept
{
    return BombDefuserOffset{
        GlobalContext::instance().clientPatternFinder("E8 ? ? ? ? 8B ? ? ? ? ? 45 33 ? 4C 8B ? ? ? ? ? 83"_pat).add(7).as<std::int32_t*>()
    };
}

inline BombDefuseEndTimeOffset PlantedC4Patterns::m_flDefuseCountDown() noexcept
{
    return BombDefuseEndTimeOffset{
        GlobalContext::instance().clientPatternFinder("74 11 F3 0F 10 B7 ? ? ? ? 48"_pat).add(6).as<std::int32_t*>()
    };
}
