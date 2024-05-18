#pragma once

#include <CS2/Classes/CCvar.h>
#include <GameClasses/OffsetTypes/CvarOffset.h>
#include <MemorySearch/BytePatternLiteral.h>

template <typename PatternFinders>
struct CvarPatterns {
    const PatternFinders& patternFinders;

    [[nodiscard]] cs2::CCvar** cvar() const noexcept
    {
        return patternFinders.clientPatternFinder("53 4C 8D 65 ? 48 83 EC ? 48 8D 05 ? ? ? ?"_pat).add(12).abs().template as<cs2::CCvar**>();
    }

    [[nodiscard]] OffsetToConVarList offsetToConVarList() const noexcept
    {
        return patternFinders.tier0PatternFinder("74 ? 0F B7 F6 48 C1 E6 04 48 03 77 ?"_pat).add(12).template readOffset<OffsetToConVarList>();
    }
};
