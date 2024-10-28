#pragma once

#include <MemoryPatterns/PatternTypes/PanelPatternTypes.h>
#include <MemorySearch/CodePattern.h>

struct PanelPatterns {
    [[nodiscard]] static consteval auto addClientPatterns(auto clientPatterns) noexcept
    {
        return clientPatterns
            .template addPattern<PanelConstructorPointer, CodePattern{"55 48 89 E5 41 56 49 89 FE 41 55 41 54 49 89 F4 48 83 EC 08 48 85 F6 74 0C 48 8B 06 48 89 F7 FF 50 50 49 89 C4 BF ? ? ? ? E8 ? ? ? ? 4C 89 F1"}>();
    }
};
