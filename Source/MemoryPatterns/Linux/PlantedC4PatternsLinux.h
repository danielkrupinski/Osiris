#pragma once

#include <GameClasses/OffsetTypes/PlantedC4Offset.h>
#include <MemorySearch/BytePatternLiteral.h>

template <typename PatternFinders>
struct PlantedC4Patterns {
    const PatternFinders& patternFinders;

    [[nodiscard]] cs2::CUtlVector<cs2::CPlantedC4*>* plantedC4s() const noexcept
    {
        return patternFinders.clientPatternFinder("? ? ? ? 8B 10 85 D2 0F 8F"_pat).abs().template as<cs2::CUtlVector<cs2::CPlantedC4*>*>();
    }

    [[nodiscard]] BombSiteOffset m_nBombSite() const noexcept
    {
        return patternFinders.clientPatternFinder("83 BF ? ? ? ? 01 0F 94 C0 C3"_pat).add(2).template readOffset<BombSiteOffset>();
    }

    [[nodiscard]] BombTickingOffset m_bBombTicking() const noexcept
    {
        return patternFinders.clientPatternFinder("00 84 C0 74 ? 80 BB ? ? ? ? 00 74 ? 80"_pat).add(7).template readOffset<BombTickingOffset>();
    }

    [[nodiscard]] BombBlowTimeOffset m_flC4Blow() const noexcept
    {
        return patternFinders.clientPatternFinder("F3 0F 10 83 ? ? ? ? F3 0F 5C C1 0F 2F E0"_pat).add(4).template readOffset<BombBlowTimeOffset>();
    }

    [[nodiscard]] BombDefuserOffset m_hBombDefuser() const noexcept
    {
        return patternFinders.clientPatternFinder("E8 ? ? ? ? 8B 8B ? ? ? ? 83 F9 FF 0F 84 ? ? ? ? 48 8B 3D"_pat).add(7).template readOffset<BombDefuserOffset>();
    }

    [[nodiscard]] BombDefuseEndTimeOffset m_flDefuseCountDown() const noexcept
    {
        return patternFinders.clientPatternFinder("74 ? F3 0F 10 80 ? ? ? ? 84"_pat).add(6).template readOffset<BombDefuseEndTimeOffset>();
    }
};
