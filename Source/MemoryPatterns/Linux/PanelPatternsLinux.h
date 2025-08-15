#pragma once

#include <MemoryPatterns/PatternTypes/PanelPatternTypes.h>
#include <MemorySearch/CodePattern.h>

struct PanelPatterns {
    [[nodiscard]] static consteval auto addClientPatterns(auto clientPatterns) noexcept
    {
        return clientPatterns
            .template addPattern<PanelConstructorPointer, CodePattern{"55 48 89 E5 41 55 49 89 FD BF ? ? ? ? 41 54 49 89 F4 53 48 83 EC ? E8 ? ? ? ? 4C 89 E9"}>();
    }
};
