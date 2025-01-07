#pragma once

#include <MemoryPatterns/PatternTypes/PanoramaDropDownPatternTypes.h>
#include <MemorySearch/CodePattern.h>

struct PanoramaDropDownPatterns {
    [[nodiscard]] static consteval auto addClientPatterns(auto clientPatterns) noexcept
    {
        return clientPatterns
            .template addPattern<SetSelectedIndexFunctionPointer, CodePattern{"55 48 89 E5 41 55 41 89 F5 41 54 48 8D 77 ? 49 89 FC 53 48 83 EC ? 8B"}>();
    }
};
