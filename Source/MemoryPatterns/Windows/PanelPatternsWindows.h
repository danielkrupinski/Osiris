#pragma once

#include <MemoryPatterns/PatternTypes/PanelPatternTypes.h>
#include <MemorySearch/CodePattern.h>

struct PanelPatterns {
    [[nodiscard]] static consteval auto addClientPatterns(auto clientPatterns) noexcept
    {
        return clientPatterns
            .template addPattern<PanelConstructorPointer, CodePattern{"48 89 5C 24 ? 48 89 74 24 ? 57 48 83 EC 30 48 8B F1 48 8B FA B9 ? ? ? ? E8 ? ? ? ? 48 8B D8 48 85 C0 74 ? 48"}>();
    }
};
