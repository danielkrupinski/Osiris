#pragma once

#include <MemoryPatterns/PatternTypes/CvarPatternTypes.h>
#include <MemorySearch/CodePattern.h>

#include <CS2/Classes/CCvar.h>
#include <GameClasses/OffsetTypes/CvarOffset.h>
#include <MemorySearch/BytePatternLiteral.h>

struct CvarPatterns2 {
    [[nodiscard]] static consteval auto addClientPatterns(auto clientPatterns) noexcept
    {
        return clientPatterns
            .template addPattern<CvarPointer, CodePattern{"48 83 EC ? ? 8B ? ? ? ? ? 48 8D 54 ? ? 4C"}.add(7).abs()>();
    }
};

template <typename PatternFinders>
struct CvarPatterns {
    const PatternFinders& patternFinders;

    [[nodiscard]] OffsetToConVarList offsetToConVarList() const noexcept
    {
        return patternFinders.tier0PatternFinder("74 ? 48 8B 41 ? 44 0F B7"_pat).add(5).template readOffset<OffsetToConVarList>();
    }
};
