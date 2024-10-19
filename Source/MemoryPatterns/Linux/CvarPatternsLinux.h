#pragma once

#include <MemoryPatterns/PatternTypes/CvarPatternTypes.h>
#include <MemorySearch/CodePattern.h>

#include <CS2/Classes/CCvar.h>
#include <GameClasses/OffsetTypes/CvarOffset.h>
#include <MemorySearch/BytePatternLiteral.h>

template <typename PatternFinders>
struct CvarPatterns {
    const PatternFinders& patternFinders;

    [[nodiscard]] OffsetToConVarList offsetToConVarList() const noexcept
    {
        return patternFinders.tier0PatternFinder("74 ? 0F B7 F6 48 C1 E6 04 48 03 77 ?"_pat).add(12).template readOffset<OffsetToConVarList>();
    }
};

struct CvarPatterns2 {
    [[nodiscard]] static consteval auto addClientPatterns(auto clientPatterns) noexcept
    {
        return clientPatterns
            .template addPattern<CvarPointer, CodePattern{"53 4C 8D 65 ? 48 83 EC ? 48 8D 05 ? ? ? ?"}.add(12).abs()>();
    }
};
