#pragma once

#include <GlobalContext.h>
#include <MemoryPatterns/PlantedC4Patterns.h>
#include <MemorySearch/BytePatternLiteral.h>

inline BombSiteOffset PlantedC4Patterns::m_nBombSite() noexcept
{
    return BombSiteOffset{
        GlobalContext::instance().clientPatternFinder("E8 ? ? ? ? 8B 55 A8 84 C0"_pat).add(1).abs().add(2).as<std::int32_t*>()
    };
}

inline BombTickingOffset PlantedC4Patterns::m_bBombTicking() noexcept
{
    return BombTickingOffset{
        GlobalContext::instance().clientPatternFinder("74 EB 41 80 BC 24 ? ? ? ? ? 0F"_pat).add(6).as<std::int32_t*>()
    };
}

inline BombBlowTimeOffset PlantedC4Patterns::m_flC4Blow() noexcept
{
    return BombBlowTimeOffset{
        GlobalContext::instance().clientPatternFinder("F3 0F 10 83 ? ? ? ? F3 0F 5C C1 0F 2F E0"_pat).add(4).as<std::int32_t*>()
    };
}

inline BombDefuserOffset PlantedC4Patterns::m_hBombDefuser() noexcept
{
    return BombDefuserOffset{
        GlobalContext::instance().clientPatternFinder("E8 ? ? ? ? 8B 8B ? ? ? ? 83 F9 FF 0F 84 ? ? ? ? 48 8B 3D"_pat).add(7).as<std::int32_t*>()
    };
}

inline BombDefuseEndTimeOffset PlantedC4Patterns::m_flDefuseCountDown() noexcept
{
    return BombDefuseEndTimeOffset{
        GlobalContext::instance().clientPatternFinder("74 ? F3 0F 10 80 ? ? ? ? 84"_pat).add(6).as<std::int32_t*>()
    };
}
