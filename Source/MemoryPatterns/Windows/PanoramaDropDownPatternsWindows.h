#pragma once

#include <MemoryPatterns/PatternTypes/PanoramaDropDownPatternTypes.h>
#include <MemorySearch/CodePattern.h>

struct PanoramaDropDownPatterns {
    [[nodiscard]] static consteval auto addClientPatterns(auto clientPatterns) noexcept
    {
        return clientPatterns
            .template addPattern<SetSelectedIndexFunctionPointer, CodePattern{"48 89 ? 24 ? 48 89 ? 24 ? ? 48 83 EC ? 48 8B ? 8B ? 48 83 C1 20 E8 ? ? ? ? 8B ? 48 8B ? 48 8B ? 08 ? 8B ? ? FF ? C0 01 00 00 48 85 ? 74 ?"}>();
    }
};
