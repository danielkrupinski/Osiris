#pragma once

#include <GameClasses/OffsetTypes/PlantedC4Offset.h>
#include <MemorySearch/BytePatternLiteral.h>

template <typename PatternFinders>
struct PlantedC4Patterns {
    const PatternFinders& patternFinders;

    [[nodiscard]] BombSiteOffset m_nBombSite() const noexcept
    {
        return patternFinders.clientPatternFinder("83 B9 ? ? ? ? 01 0F 94 C0 C3"_pat).add(2).template readOffset<BombSiteOffset>();
    }

    [[nodiscard]] BombTickingOffset m_bBombTicking() const noexcept
    {
        return patternFinders.clientPatternFinder("74 ? 44 38 AE ? ? ? ? 74 ? 49"_pat).add(5).template readOffset<BombTickingOffset>();
    }

    [[nodiscard]] BombBlowTimeOffset m_flC4Blow() const noexcept
    {
        return patternFinders.clientPatternFinder("F3 0F 10 ? ? ? ? ? BB FF"_pat).add(4).template readOffset<BombBlowTimeOffset>();
    }

    [[nodiscard]] BombDefuserOffset m_hBombDefuser() const noexcept
    {
        return patternFinders.clientPatternFinder("E8 ? ? ? ? 8B ? ? ? ? ? 45 33 ? 4C 8B ? ? ? ? ? 83"_pat).add(7).template readOffset<BombDefuserOffset>();
    }

    [[nodiscard]] BombDefuseEndTimeOffset m_flDefuseCountDown() const noexcept
    {
        return patternFinders.clientPatternFinder("74 11 F3 0F 10 B7 ? ? ? ? 48"_pat).add(6).template readOffset<BombDefuseEndTimeOffset>();
    }
};
