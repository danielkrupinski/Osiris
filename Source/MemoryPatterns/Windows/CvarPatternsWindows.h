#pragma once

#include <CS2/Classes/CCvar.h>
#include <GameClasses/OffsetTypes/CvarOffset.h>
#include <MemorySearch/BytePatternLiteral.h>

template <typename PatternFinders>
struct CvarPatterns {
    const PatternFinders& patternFinders;

    [[nodiscard]] cs2::CCvar** cvar() const noexcept
    {
        return patternFinders.clientPatternFinder("48 83 EC ? ? 8B ? ? ? ? ? 48 8D 54 ? ? 4C"_pat).add(7).abs().template as<cs2::CCvar**>();
    }

    [[nodiscard]]  OffsetToConVarList offsetToConVarList() const noexcept
    {
        return patternFinders.tier0PatternFinder("74 ? 48 8B 41 ? 44 0F B7"_pat).add(5).template readOffset<OffsetToConVarList>();
    }
};
