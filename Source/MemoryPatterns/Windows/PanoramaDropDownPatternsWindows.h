#pragma once

#include <MemoryPatterns/PatternTypes/PanoramaDropDownPatternTypes.h>
#include <MemorySearch/CodePattern.h>

struct PanoramaDropDownPatterns {
    [[nodiscard]] static consteval auto addClientPatterns(auto clientPatterns) noexcept
    {
        return clientPatterns
            .template addPattern<SetSelectedIndexFunctionPointer, CodePattern{"48 89 5C 24 ? 48 89 74 24 ? 57 48 83 EC ? 8B 05 ? ? ? ? 8B"}>();
    }
};
